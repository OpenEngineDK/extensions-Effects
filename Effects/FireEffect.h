#ifndef _OEPARTICLE_FIRE_EFFECT_H_
#define _OEPARTICLE_FIRE_EFFECT_H_

// base particle system
#include <ParticleSystem/ParticleSystem.h>
#include <ParticleSystem/ParticleCollection.h>
#include <ParticleSystem/IParticleEffect.h>

// particle types
#include <ParticleSystem/Particles/IParticle.h>
#include <ParticleSystem/Particles/Position.h>
#include <ParticleSystem/Particles/PreviousPosition.h>
#include <ParticleSystem/Particles/Life.h>
#include <ParticleSystem/Particles/Color.h>
#include <ParticleSystem/Particles/Size.h>
#include <ParticleSystem/Particles/Texture.h>

// predefined modifiers
#include <ParticleSystem/StaticForceModifier.h>
#include <ParticleSystem/LinearColorModifier.h>
#include <ParticleSystem/SizeModifier.h>
#include <ParticleSystem/LifespanModifier.h>
#include <ParticleSystem/VerletModifier.h>
#include <ParticleSystem/TextureRotationModifier.h>

// predefined initializers
#include <ParticleSystem/RandomTextureInitializer.h>

#include <Renderers/IRenderer.h>
#include <Renderers/IRenderingView.h>
#include <Renderers/IRenderNode.h>
#include <Scene/ISceneNode.h>

#include <Renderers/OpenGL/TextureLoader.h>
#include <Resources/ITextureResource.h>
#include <Resources/ResourceManager.h>

#include <Meta/OpenGL.h>

#include <Math/RandomGenerator.h>

namespace OpenEngine {
    namespace Effects {

using namespace Renderers;
using namespace Scene;
using namespace ParticleSystem;
using namespace Resources;
using namespace Renderers::OpenGL;
using namespace Math;

typedef Color < Texture <Size < PreviousPosition < Position < Life < IParticle > > > > > >  TYPE;

class FireEffect : public IParticleEffect, public TransformationNode {
private:
    class ParticleRenderer: public IRenderNode {
    public:
        ParticleRenderer(ParticleCollection<TYPE>* particles):
            particles(particles) {}
        virtual ~ParticleRenderer() {}

        void Apply(IRenderingView* view) {
        
            // @todo: we need to move all this gl specific code into the renderer
            // @todo: also glpushmatrix is too expensive. let's make our own calculations 
            //        of the rotation and position of the quad.
            
            glPushAttrib(GL_LIGHTING);    
            glDisable(GL_LIGHTING);
            glDepthMask(GL_FALSE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_TEXTURE_2D);
            glEnable(GL_COLOR_MATERIAL);
    
            for (particles->iterator.Reset(); particles->iterator.HasNext(); particles->iterator.Next()) {
                TYPE& particle = particles->iterator.Element();
                ITextureResourcePtr texr = particle.texture;
            
                //Set texture
                if (texr != NULL) {
                    if (texr->GetID() == 0) {
                        TextureLoader::LoadTextureResource(texr);
                        //logger.info << texr->GetID() << logger.end;
                    }
                    glBindTexture(GL_TEXTURE_2D, texr->GetID());
                }
            
                else {
                    glBindTexture(GL_TEXTURE_2D, 0);
                }
            
                glPushMatrix();
                glTranslatef(particle.position[0], particle.position[1], particle.position[2]);
            
                // billboard
                float modelview[16];
                glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
                modelview[0] = modelview[5] = modelview[10] = 1.0;
                modelview[1] = modelview[2] = modelview[4] = 
                    modelview[6] = modelview[8] = modelview[9] = 0.0; 
                glLoadMatrixf(modelview);

                // apply quad transformations
                glRotatef(particle.rotation, 0,0,1);
                float scale = particle.size;
                glScalef(scale,scale,scale);
                float c[4];
                particle.color.ToArray(c);
                glColor4fv(c);
            
                glBegin(GL_QUADS);
                glTexCoord2f(0.0, 0.0);
                glVertex3f(-1.0, -1.0, 0.0);
                glTexCoord2f(0.0, 1.0);
                glVertex3f(-1.0, 1.0, 0);
                glTexCoord2f(1.0, 1.0);
                glVertex3f(1, 1, 0);
                glTexCoord2f(1.0, 0.0);
                glVertex3f(1, -1, 0);
                glEnd();
            
                glPopMatrix();
            }
        
            glDisable(GL_BLEND);
            glPopAttrib();
        
            // render subnodes
            VisitSubNodes(*view);      
        }

    private:
        ParticleCollection<TYPE>* particles;
    };
    
    ParticleRenderer* pr;
    
    ParticleCollection<TYPE>* particles;

    OpenEngine::ParticleSystem::ParticleSystem* system;

    //initializers
    RandomTextureInitializer<TYPE> inittex;

    //modifiers
    VerletModifier<TYPE> verlet;
    StaticForceModifier<TYPE> wind, antigravity;
    SizeModifier<TYPE> sizemod;
    LinearColorModifier<TYPE> colormod;
    LifespanModifier<TYPE> lifemod;
    TextureRotationModifier<TYPE> rotationmod;

    RandomGenerator randomgen;
    

public:
    FireEffect(OpenEngine::ParticleSystem::ParticleSystem* system): 
        system(system),
        wind(Vector<3,float>(1.591,0,0)),
        antigravity(Vector<3,float>(0,0.382,0)),
        sizemod(2.0){
        particles = system->CreateParticles<TYPE>(500);     
        
        //load texture resource
        ITextureResourcePtr texr1 = ResourceManager<ITextureResource>::Create("Smoke/smoke01.tga");
        ITextureResourcePtr texr2 = ResourceManager<ITextureResource>::Create("Smoke/smoke02.tga");
        ITextureResourcePtr texr3 = ResourceManager<ITextureResource>::Create("Smoke/smoke03.tga");

        //inittex.AddTextureResource(texr1);
        //inittex.AddTextureResource(texr2);
        inittex.AddTextureResource(texr3);
        
        randomgen.SeedWithTime();
        
        pr = new ParticleRenderer(particles);
}

~FireEffect() {
    delete[] particles;
}
 
void Handle(ParticleEventArg e) {
    Emit();

    for (particles->iterator.Reset(); 
         particles->iterator.HasNext(); 
         particles->iterator.Next()) {
        TYPE& particle = particles->iterator.Element();
        
        // custom modify particles

        // predefined particle modifiers

        // wind.Process(e.dt, particle);
        // antigravity.Process(e.dt, particle);
        sizemod.Process(particle);
        verlet.Process(e.dt, particle);
        colormod.Process(particle);
        rotationmod.Process(particle);
        lifemod.Process(e.dt, particles->iterator);
    }
}

inline float RandomAttribute(float base, float variance) {
    return base + randomgen.UniformFloat(-1.0,1.0) * variance;

}

void inline Emit() {
    if (particles->GetActiveParticles() >= particles->GetSize())
        return;
    
    // initializer variables
    static const float number = 7;
    static const float numberVar = 2;
    
    // attributes for emission on square
    Vector<3,float> position;
    Quaternion<float> direction;
    GetAccumulatedTransformations(&position, &direction);

    //static const Vector<3,float> position(0.0, -30.0, -50.0);
    //static const Vector<3,float> devAxis1(20.0,0.0,0.0);
    //static const Vector<3,float> devAxis2(0.0,0.0,20.0);        
    
    static const float life = 2100;
    static const float lifeVar = 1000;
    
    static const float size = 1;
    static const float sizeVar = 0;
    
    // angle is the angular deviation from the direction of
    // the velocity
    static const float angle = 0.25*PI;
    static const float angleVar = 0.1;
    
    static const float spin = 0.05;
    static const float spinVar = 0.1;
    

    int emits = min(unsigned(round(RandomAttribute(number, numberVar))),
                    particles->GetSize()-particles->GetActiveParticles());
    
    for (int i = 0; i < emits; i++) {
        TYPE& particle = particles->NewParticle();
        
        // static pos
        particle.position = position;
        //+ devAxis1*randomgen.UniformFloat(-1.0,1.0) 
        //    + devAxis2*randomgen.UniformFloat(-1.0,1.0);
            
        particle.life = 0;
        particle.maxlife = RandomAttribute(life, lifeVar);
        particle.size = particle.startsize = RandomAttribute(size, sizeVar);
        particle.rotation = 0;
        particle.spin = RandomAttribute(spin, spinVar);

        //color
        particle.color = particle.startColor = Vector<4,float>(0.85,0.1,0.0,0.8);
        particle.endColor = Vector<4,float>(0.1,0.1,0.1,0.1);

        // texture
        inittex.Process(particle);
    
        // random direction
//         float r = (rand()/((float)RAND_MAX))*RandomAttribute(angle, angleVar);
//         float p = (rand()/((float)RAND_MAX))*2*PI;
//         Quaternion<float> q(r,p, 0);
//         q.Normalize();
    
        // set the previous position
        // this will represent direction and speed when using verlet 
        // integration for updating position
        particle.previousPosition = particle.position - 
            (direction.RotateVector(Vector<3,float>(0.0,-1.0,0.0)));
    }
}

ISceneNode* GetSceneNode() {
    return pr;
}

};

}
}
#endif
