
/* Copyright (c) 2011-2014, Stefan Eilemann <eile@eyescale.ch>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * - Neither the name of Eyescale Software GmbH nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SEQ_SPLOTCH_APPLICATION_H
#define SEQ_SPLOTCH_APPLICATION_H

#include <seq/sequel.h>

#include <frameData.h>

#include <splotch/scenemaker.h>
#include <splotch/splotch_host.h>

/** The Sequel polygonal rendering example. */
namespace seqSplotch
{

using eqPly::FrameData;

class Model
{
public:
    Model()
        : params( "/home/nachbaur/dev/viz.stable/splotch/configs/fivox.par", false )
        , sMaker( params )
        , cpuRender( false )
    {
        get_colourmaps( params, amap );
        sMaker.getNextScene( particle_data, r_points, campos, centerpos, lookat, sky, outfile );

        tsize npart = particle_data.size();
        bool boost = params.find<bool>("boost",false);
        b_brightness = boost ? float(npart)/float(r_points.size()) : 1.0;
    }
    paramfile params;
    sceneMaker sMaker;
    std::vector< particle_sim > particle_data;
    std::vector< particle_sim > r_points;
    std::vector< COLOURMAP > amap;
    vec3 campos, centerpos, lookat, sky;
    std::string outfile;
    float b_brightness;

    bool cpuRender;
};

class Application : public seq::Application
{
public:
    Application() /*: _model( 0 ), _modelDist( 0 )*/ {}

    bool init( const int argc, char** argv );
    bool run();
    bool exit() final;
    void onStartFrame() final;

    seq::Renderer* createRenderer()  final;
    co::Object* createObject( const uint32_t type )  final;

    Model& getModel(/* const eq::uint128_t& modelID */);

private:
    FrameData _frameData;
    std::unique_ptr< Model > _model;
//    ModelDist* _modelDist;
//    lunchbox::Lock _modelLock;

    virtual ~Application() {}
//    eq::Strings _parseArguments( const int argc, char** argv );
    void _loadModel(/* const eq::Strings& models */);
//    void _unloadModel();
};

typedef lunchbox::RefPtr< Application > ApplicationPtr;
}

#endif // SEQ_SPLOTCH_APPLICATION_H
