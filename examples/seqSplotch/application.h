
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
        , cpuRender( true )
    {
        get_colourmaps( params, amap );
        sMaker.getNextScene( particle_data, r_points, campos, centerpos, lookat, sky, outfile );

        tsize npart = particle_data.size();
        _boost = params.find<bool>("boost",false);
        b_brightness = _boost ? float(npart)/float(r_points.size()) : 1.0;

        unsigned numTypes = params.find<int>("ptypes",1);

        for(unsigned i = 0; i < numTypes; i++)
        {
            brightness.push_back(params.find<float>("brightness"+dataToString(i),1.f) * params.find<float>("pv_brightness_mod"+dataToString(i),1.f));
            //colour_is_vec.push_back(splotchParams->find<bool>("color_is_vector"+dataToString(i),0));
            smoothing_length.push_back(params.find<float>("size_fix"+dataToString(i),0) );
        }

        // Ensure unused elements up to tenth are 1 (static size 10 array in shader)
        if(brightness.size()<10)
            brightness.resize(10,1);

        if(smoothing_length.size()<10)
            smoothing_length.resize(10,0);

        radial_mod = params.find<double>("pv_radial_mod",1.f);
    }

    std::vector< particle_sim > getParticles() const
    {
        return _boost ? r_points : particle_data;
    }

    seq::Matrix4f getModelMatrix() const
    {
        const seq::Vector3f eye( campos.x, campos.y, campos.z );
        const seq::Vector3f center( lookat.x, lookat.y, lookat.z );
        const seq::Vector3f up( sky.x, sky.y, sky.z );
        return seq::Matrix4f( eye, center, up );
    }

    paramfile params;
    sceneMaker sMaker;
    std::vector< particle_sim > particle_data;
    std::vector< particle_sim > r_points;
    std::vector< COLOURMAP > amap;
    vec3 campos, centerpos, lookat, sky;
    std::string outfile;
    bool _boost;
    float b_brightness;
    std::vector<particle_sim>* pData;

    // gpu only
    std::vector<float> brightness;
    std::vector<float> smoothing_length;
    float radial_mod;

    bool cpuRender;
};

class ViewData : public seq::ViewData
{
public:
    ViewData( const seq::Matrix4f& initialModelMatrix )
        : _initialModelMatrix( initialModelMatrix )
    {
        setModelMatrix( _initialModelMatrix );
    }
    ~ViewData() {}

    bool handleEvent( const eq::ConfigEvent* event_ ) final
    {
        const eq::Event& event = event_->data;
        switch( event.type )
        {
        case eq::Event::KEY_PRESS:
            switch( event.keyPress.key )
            {
            case ' ':
                setModelMatrix( _initialModelMatrix );
                return true;
            }
        }
        return seq::ViewData::handleEvent( event_ );
    }

private:
    const seq::Matrix4f _initialModelMatrix;
};

class Application : public seq::Application
{
public:
    Application();

    bool init( const int argc, char** argv );
    bool run();
    bool exit() final;

    seq::Renderer* createRenderer()  final;
    co::Object* createObject( const uint32_t type )  final;

    Model& getModel(/* const eq::uint128_t& modelID */);

private:
    seq::ViewData* createViewData() final;
    void destroyViewData( seq::ViewData* viewData ) final;

    FrameData _frameData;
    std::unique_ptr< Model > _model;
//    ModelDist* _modelDist;
//    lunchbox::Lock _modelLock;

    virtual ~Application() {}
//    eq::Strings _parseArguments( const int argc, char** argv );
    void _loadModel(/* const eq::Strings& models */);
//    void _unloadModel();

    ViewData* _viewData;
};

typedef lunchbox::RefPtr< Application > ApplicationPtr;
}

#endif // SEQ_SPLOTCH_APPLICATION_H
