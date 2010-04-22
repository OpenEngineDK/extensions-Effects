#ifndef _OEPARTICLE_TEXT_EFFECT_H_
#define _OEPARTICLE_TEXT_EFFECT_H_

// base particle system
#include <ParticleSystem/ParticleSystem.h>
#include <ParticleSystem/ParticleCollection.h>
#include <ParticleSystem/IParticleEffect.h>

// particle types
#include <ParticleSystem/Particles/IParticle.h>
#include <ParticleSystem/Particles/Position.h>
#include <ParticleSystem/Particles/Life.h>
#include <ParticleSystem/Particles/Color.h>
#include <ParticleSystem/Particles/Size.h>
#include <ParticleSystem/Particles/Texture.h>
#include <ParticleSystem/Particles/Forces.h>
#include <ParticleSystem/Particles/Velocity.h>

// predefined modifiers
#include <ParticleSystem/StaticForceModifier.h>
#include <ParticleSystem/LifespanModifier.h>
#include <ParticleSystem/EulerModifier.h>
#include <ParticleSystem/LinearValueModifier.h>

// predefined initializers
// ----

#include <Renderers/IRenderer.h>
#include <Renderers/IRenderingView.h>
#include <Scene/RenderNode.h>
#include <Scene/ISceneNode.h>

#include <Renderers/TextureLoader.h>
#include <Resources/ITexture2D.h>
#include <Resources/ResourceManager.h>

#include <Meta/OpenGL.h>

#include <Math/RandomGenerator.h>

#include <Scene/TransformationNode.h>

namespace OpenEngine {
    namespace Effects {

using namespace Renderers;
using namespace Scene;
using namespace ParticleSystem;
using namespace Resources;
using namespace Math;

class TextEffect : public IParticleEffect {
public:
    typedef Color < Texture <Size < Velocity < Forces < Position < Life < IParticle > > > > > > >  TYPE;

private:
    class ParticleRenderer: public RenderNode {
    public:
        ParticleRenderer(ParticleCollection<TYPE>* particles, Renderers::TextureLoader& textureLoader):
            particles(particles), textureLoader(textureLoader) {}
        virtual ~ParticleRenderer() {}

        void Apply(RenderingEventArg arg, ISceneNodeVisitor& v) {
        
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
    
            for (particles->iterator.Reset(); 
                 particles->iterator.HasNext(); 
                 particles->iterator.Next()) {
                
                TYPE& particle = particles->iterator.Element();
                ITexture2DPtr texr = particle.texture;
                
                //Set texture
                if (texr != NULL) {
                    if (texr->GetID() == 0) {
                        textureLoader.Load(texr);
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
            glPopAttrib();
            glDisable(GL_BLEND);
            CHECK_FOR_GL_ERROR();
            
            // render subnodes
            VisitSubNodes(v);      
        }

    private:
        ParticleCollection<TYPE>* particles;
        TextureLoader& textureLoader; 
    };

protected:
    ParticleCollection<TYPE>* particles;
    
    // emit attributes
    float life;
    float lifeVar;
    
    // angle is the angular deviation from the direction of
    // the velocity
    float angle;
    
    float speed;
    float speedVar;
    
    OpenEngine::ParticleSystem::ParticleSystem& system;
    
    bool active;
    
    ParticleRenderer* pr;
    
    //modifiers
    EulerModifier<TYPE> eulermod;
    LifespanModifier<TYPE> lifemod;
    LinearValueModifier<TYPE, Vector<4,float> > cmod;
    LinearValueModifier<TYPE, float> sizemod;
    StaticForceModifier<TYPE> gravity;
    
    RandomGenerator randomgen;
    TransformationNode* transPos;

    ITexture2DPtr tex;
    
public:
    TextEffect(OpenEngine::ParticleSystem::ParticleSystem& system,
               unsigned int numParticles,
               float life, float lifeVar,
               float speed, float speedVar,
               Vector<3,float> gravity,
               Renderers::TextureLoader& textureLoader): 
        particles(system.CreateParticles<TYPE>(numParticles)),
        life(life), lifeVar(lifeVar),
        speed(speed), speedVar(speedVar),
        system(system),
        active(false),
        pr(new ParticleRenderer(particles, textureLoader)),
        gravity(gravity),
        transPos(NULL)
    {
        randomgen.SeedWithTime();
     
    }
    
    TextEffect(OpenEngine::ParticleSystem::ParticleSystem& system, 
               TextureLoader& textureLoader): 
        particles(system.CreateParticles<TYPE>(49)),
        life(6.1),
        lifeVar(0.5),
        speed(10),
        speedVar(1),
        system(system),
        active(false),
        pr(new ParticleRenderer(particles, textureLoader)),
        gravity(Vector<3,float>(0,-1.42,0)),
        transPos(NULL)
    {        
        tex = 
        ResourceManager<ITexture2D>::Create("1.tga");


        randomgen.SeedWithTime();
        cmod.AddValue(1.0, Vector<4,float>(.1,.9,.0,.0));
        cmod.AddValue(0.0, Vector<4,float>(.1,.9,.0,1.0));
        sizemod.AddValue(1.0, 0.01);
        sizemod.AddValue(.9, 1.2);
        sizemod.AddValue(0.0, 0.5);
}

~TextEffect() {
    delete particles;
}

void Handle(ParticleEventArg e) {
    for (particles->iterator.Reset(); 
         particles->iterator.HasNext(); 
         particles->iterator.Next()) {
        TYPE& particle = particles->iterator.Element();
        
        // predefined particle modifiers
        gravity.Process(e.dt, particle);
        eulermod.Process(e.dt, particle);
        sizemod.Process(e.dt, particle, particle.size);
        cmod.Process(e.dt, particle, particle.color);
        lifemod.Process(e.dt, particle);
        
        if (particle.life >= particle.maxlife)
            particles->iterator.Delete();
    }
}

inline float RandomAttribute(float base, float variance) {
    return base + randomgen.UniformFloat(-1.0,1.0) * variance;
}

void inline Emit() {
//     if (particles->GetActiveParticles() >= particles->GetSize())
//         return;
 
    if (particles->GetSize() == particles->GetActiveParticles())
        return;
   
    Vector<3,float> position;
    Quaternion<float> direction;
    if (transPos)
        transPos->GetAccumulatedTransformations(&position, &direction);

    TYPE& particle = particles->NewParticle();
    
    // position based on transformation hierarchy
    particle.position = position;
    
    particle.life = 0;
    particle.maxlife = RandomAttribute(life, lifeVar);
    particle.size = 0;
    particle.rotation = 0;
    particle.texture = tex;
    
    // set velocity and forces for use with euler integration
    particle.velocity = direction.RotateVector(Vector<3,float>(0.0,-1.0,0.0))
                                       *RandomAttribute(speed,speedVar);
    particle.forces = Vector<3,float>(0.0,0.0,0.0);
}

ISceneNode* GetSceneNode() {
    return pr;
}

void SetActive(bool active) {
    this->active = active;
}

bool GetActive() {
    return active;
}

void Reset() {
}

TransformationNode* GetTransformationNode() {
    return transPos;
}

void SetTransformationNode(TransformationNode* node) {
    transPos = node;
}

void EmitText(string s, TransformationNode* pos) {
    transPos = pos;
    Emit();
    transPos = NULL;
}

};

}
}
#endif
