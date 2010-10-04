
/* Copyright (c) 2007-2010, Stefan Eilemann <eile@equalizergraphics.com> 
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 2.1 as published
 * by the Free Software Foundation.
 *  
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "unbufferedMasterCM.h"

#include "command.h"
#include "commands.h"
#include "log.h"
#include "node.h"
#include "object.h"
#include "objectDeltaDataOStream.h"
#include "objectInstanceDataOStream.h"
#include "packets.h"
#include "session.h"

namespace eq
{
namespace net
{
typedef CommandFunc<UnbufferedMasterCM> CmdFunc;

UnbufferedMasterCM::UnbufferedMasterCM( Object* object )
        : MasterCM( object )
{
    _version = 1;
    registerCommand( CMD_OBJECT_COMMIT, 
                     CmdFunc( this, &UnbufferedMasterCM::_cmdCommit ), 0 );
    // sync commands are send to any instance, even the master gets the command
    registerCommand( CMD_OBJECT_DELTA,
                     CmdFunc( this, &UnbufferedMasterCM::_cmdDiscard ), 0 );
}

UnbufferedMasterCM::~UnbufferedMasterCM()
{}

uint32_t UnbufferedMasterCM::addSlave( Command& command )
{
    EQ_TS_THREAD( _cmdThread );
    EQASSERT( command->type == PACKETTYPE_EQNET_SESSION );
    EQASSERT( command->command == CMD_SESSION_SUBSCRIBE_OBJECT );

    NodePtr node = command.getNode();
    SessionSubscribeObjectPacket* packet =
        command.getPacket<SessionSubscribeObjectPacket>();
    const uint32_t version = packet->requestedVersion;
    const uint32_t instanceID = packet->instanceID;

    EQASSERT( version == VERSION_OLDEST || version == VERSION_NONE ||
              version == _version );

    // add to subscribers
    ++_slavesCount[ node->getNodeID() ];
    _slaves.push_back( node );
    stde::usort( _slaves );

#if 0
    EQLOG( LOG_OBJECTS ) << "Object id " << _object->_id << " v" << _version
                         << ", instantiate on " << node->getNodeID()
                         << std::endl;
#endif
    const bool useCache = packet->masterInstanceID == _object->getInstanceID();

    if( useCache && 
        packet->minCachedVersion <= _version && 
        packet->maxCachedVersion >= _version )
    {
#ifdef EQ_INSTRUMENT_MULTICAST
        ++_hit;
#endif
        return ( version == VERSION_OLDEST ) ? 
            packet->minCachedVersion : _version;
    }

    // send instance data
    ObjectInstanceDataOStream os( this );
    os.setVersion( _version );
    os.setInstanceID( instanceID );
    os.setNodeID( node->getNodeID( ));

    if( version != VERSION_NONE ) // send current data
    {
        os.enable( node, true );
        _object->getInstanceData( os );
        os.disable();
    }

    if( !os.hasSentData( )) // if no data, send empty packet to set version
    {
        ObjectInstancePacket instancePacket;
        instancePacket.nChunks = 0;
        instancePacket.last = true;
        instancePacket.version = _version;
        instancePacket.dataSize = 0;
        instancePacket.instanceID = instanceID;
        instancePacket.masterInstanceID = _object->getInstanceID();
        _object->send( node, instancePacket );
    }
#ifdef EQ_INSTRUMENT_MULTICAST
    ++_miss;
#endif
    return VERSION_INVALID; // no data was in cache
}

void UnbufferedMasterCM::removeSlave( NodePtr node )
{
    EQ_TS_THREAD( _cmdThread );
    // remove from subscribers
    const NodeID& nodeID = node->getNodeID();
    EQASSERTINFO( _slavesCount[ nodeID ] != 0, base::className( _object ));

    --_slavesCount[ nodeID ];
    if( _slavesCount[ nodeID ] == 0 )
    {
        Nodes::iterator i = find( _slaves.begin(), _slaves.end(), node );
        EQASSERT( i != _slaves.end( ));
        _slaves.erase( i );
        _slavesCount.erase( nodeID );
    }
}

//---------------------------------------------------------------------------
// command handlers
//---------------------------------------------------------------------------
bool UnbufferedMasterCM::_cmdCommit( Command& command )
{
    EQ_TS_THREAD( _cmdThread );
    NodePtr localNode = _object->getLocalNode();
    const ObjectCommitPacket* packet = command.getPacket<ObjectCommitPacket>();
#if 0
    EQLOG( LOG_OBJECTS ) << "commit v" << _version << " " << command
                         << std::endl;
#endif
    if( _slaves.empty( ))
    {
        localNode->serveRequest( packet->requestID, _version );
        return true;
    }

    ObjectDeltaDataOStream os( this );

    os.setVersion( _version + 1 );
    os.enable( _slaves );
    _object->pack( os );
    os.disable();

    if( os.hasSentData( ))
    {
        ++_version;
        EQASSERT( _version );
#if 0
        EQLOG( LOG_OBJECTS ) << "Committed v" << _version << ", id " 
                             << _object->getID() << std::endl;
#endif
    }

    localNode->serveRequest( packet->requestID, _version );
    return true;
}

}
}
