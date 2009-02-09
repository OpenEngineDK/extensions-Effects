#ifndef _OEPARTICLE_FIRE_EFFECT_H_
#define _OEPARTICLE_FIRE_EFFECT_H_

// base particle system
#include <ParticleSystem/ParticleSystem.h>
#include <ParticleSystem/ParticleCollection.h>
#include <ParticleSystem/IParticleEffect.h>

// particle types
#include <ParticleSystem/Particles/IParticle.h>
#include <ParticleSystem/Particles/Position.h>
//#include <ParticleSystem/Particles/PreviousPosition.h>
#include <ParticleSystem/Particles/Life.h>
#include <ParticleSystem/Particles/Color.h>
#include <ParticleSystem/Particles/Size.h>
#include <ParticleSystem/Particles/Texture.h>
#include <ParticleSystem/Particles/Forces.h>
#include <ParticleSystem/Particles/Velocity.h>

// predefined modifiers
#include <ParticleSystem/StaticForceModifier.h>
#include <ParticleSystem/LinearColorModifier.h>
#include <ParticleSystem/SizeModifier.h>
#include <ParticleSystem/LifespanModifier.h>
#include <ParticleSystem/EulerModifier.h>
//#include <ParticleSystem/VerletModifier.h>
#include <ParticleSystem/TextureRotationModifier.h>
#include <ParticleSystem/ColorModifier.h>

// predefined initializers
#include <ParticleSystem/RandomTextureInitializer.h>

#include <Renderers/IRenderer.h>
#include <Renderers/IRenderingView.h>
#include <Scene/RenderNode.h>
#include <Scene/ISceneNode.h>

#include <Renderers/TextureLoader.h>
#include <Resources/ITextureResource.h>
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
        //using namespace Renderers::OpenGL;
using namespace Math;

class FireEffect : public IParticleEffect {
public:
    typedef Color < Texture <Size < Velocity < Forces < Position < Life < IParticle > > > > > > >  TYPE;

protected:
    ParticleCollection<TYPE>* particles;

private:
    class ParticleRenderer: public RenderNode {
    public:
        ParticleRenderer(ParticleCollection<TYPE>* particles, Renderers::TextureLoader& textureLoader):
            particles(particles), textureLoader(textureLoader) {}
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
    
            for (particles->iterator.Reset(); 
                 particles->iterator.HasNext(); 
                 particles->iterator.Next()) {
                
                TYPE& particle = particles->iterator.Element();
                ITextureResourcePtr texr = particle.texture;
                
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
            VisitSubNodes(*view);      
        }

    private:
        ParticleCollection<TYPE>* particles;
        TextureLoader& textureLoader; 
    };

    unsigned int totalEmits;


    
protected:
    // emit attributes
    float number;
    float numberVar;

    float life;
    float lifeVar;
    
    float size;
    float sizeVar;
    
    // angle is the angular deviation from the direction of
    // the velocity
    float angle;
    
    float spin;
    float spinVar;
    
    float speed;
    float speedVar;

    //color
    Vector<4,float> startColor;
    Vector<4,float> endColor;

    OpenEngine::ParticleSystem::ParticleSystem& system;

    bool active;
    
    ParticleRenderer* pr;

    //initializers
    RandomTextureInitializer<TYPE> inittex;

    //modifiers
    //    VerletModifier<TYPE> verlet;
    EulerModifier<TYPE> euler;
    StaticForceModifier<TYPE> antigravity;
    SizeModifier<TYPE> sizemod;
    LinearColorModifier<TYPE> lcm;
    LifespanModifier<TYPE> lifemod;
    TextureRotationModifier<TYPE> rotationmod;
    ColorModifier<TYPE> cmod;

    RandomGenerator randomgen;
    TransformationNode* transPos;

public:
    FireEffect(OpenEngine::ParticleSystem::ParticleSystem& system,
               unsigned int numParticles,
               float number, float numberVar,
               float life, float lifeVar,
               float size, float sizeVar, float maxSize,
               float angle, 
               float spin, float spinVar,
               float speed, float speedVar,
               Vector<4,float> startColor, Vector<4,float> endColor,
               Vector<3,float> antigravity,
               Renderers::TextureLoader& textureLoader): 
        particles(system.CreateParticles<TYPE>(numParticles)),
        totalEmits(0),
        number(number), numberVar(numberVar),
        life(life), lifeVar(lifeVar),
        size(size), sizeVar(sizeVar),
        angle(angle),
        spin(spin), spinVar(spinVar),
        speed(speed), speedVar(speedVar),
        startColor(startColor), endColor(endColor),
        system(system),
        active(false),
        pr(new ParticleRenderer(particles, textureLoader)),
        antigravity(antigravity),
        sizemod(maxSize),
        transPos(NULL) 
    {
        randomgen.SeedWithTime();

        // color sequence
        cmod.AddColor(1.0, Vector<4,float>(0.1,0.01,0.01,.3)); // blackish
        cmod.AddColor(.7, Vector<4,float>(.7,0.3,0.1,.6));     // redish
        cmod.AddColor(.2, Vector<4,float>(.9,0.75,0.2,.7));    // orangeish
        cmod.AddColor(.0, Vector<4,float>(0.2,0.2,0.3,.1));    // blueish
    }
    
    FireEffect(OpenEngine::ParticleSystem::ParticleSystem& system, 
               TextureLoader& textureLoader): 
        particles(system.CreateParticles<TYPE>(200)),
        totalEmits(0),
        number(7.0),
        numberVar(2.0),
        life(2100.0),
        lifeVar(500.0),
        size(2.0),
        sizeVar(0.5),
        angle(0.1),
        spin(0.09),
        spinVar(0.1),
        speed(1.7),
        speedVar(0.25),
        startColor(Vector<4,float>(.9,.9,0.0,0.9)),
        endColor(Vector<4,float>(0.8,0.0,0.0,0.3)),
        system(system),
        active(false),
        pr(new ParticleRenderer(particles, textureLoader)),
        antigravity(Vector<3,float>(0,0.182,0)),
        sizemod(5.0),
        transPos(NULL)
    {        
        
        randomgen.SeedWithTime();
        cmod.AddColor(1.0, Vector<4,float>(0.1,0.01,0.01,.3)); // blackish
        cmod.AddColor(.7, Vector<4,float>(.7,0.3,0.1,.6));     // redish
        cmod.AddColor(.2, Vector<4,float>(.9,0.75,0.2,.7));   // orangeish
        cmod.AddColor(.0, Vector<4,float>(0.2,0.2,0.3,.2));   // blueish
}

~FireEffect() {
    delete particles;
}

void Handle(ParticleEventArg e) {
    if (active) {
        totalEmits += Emit();
    }

    for (particles->iterator.Reset(); 
         particles->iterator.HasNext(); 
         particles->iterator.Next()) {
        TYPE& particle = particles->iterator.Element();
        
        // custom modify particles
        
        // predefined particle modifiers
        
        //wind.Process(e.dt, particle);
        antigravity.Process(e.dt, particle);
        sizemod.Process(particle);
        euler.Process(e.dt, particle);
        cmod.Process(e.dt, particle);
        rotationmod.Process(particle);
        lifemod.Process(e.dt, particle);
        
        if (particle.life >= particle.maxlife)
            particles->iterator.Delete();
    }
}

inline float RandomAttribute(float base, float variance) {
    return base + randomgen.UniformFloat(-1.0,1.0) * variance;

}

unsigned int GetTotalEmits() {
    return totalEmits;
}

unsigned int inline Emit() {
//     if (particles->GetActiveParticles() >= particles->GetSize())
//         return;
    
    Vector<3,float> position;
    Quaternion<float> direction;
    if (transPos)
        transPos->GetAccumulatedTransformations(&position, &direction);    

    unsigned int emits = min(unsigned(round(RandomAttribute(number, numberVar))),
                    particles->GetSize()-particles->GetActiveParticles());
    
    for (unsigned int i = 0; i < emits; i++) {
        TYPE& particle = particles->NewParticle();
        
        // position based on transformation hierarchy (point emission)
        particle.position = position;
        
        particle.life = 0;
        particle.maxlife = RandomAttribute(life, lifeVar);
        particle.size = particle.startsize = RandomAttribute(size, sizeVar);
        particle.rotation = 0;
        particle.spin = RandomAttribute(spin, spinVar);

        //color
        particle.color = particle.startColor = startColor;
        particle.endColor = endColor;

        // texture
        inittex.Process(particle);
    
        // random direction
        float r = randomgen.UniformFloat(-1.0,1.0)*angle;
        float p = randomgen.UniformFloat(-1.0,1.0)*angle;
        float y = randomgen.UniformFloat(-1.0,1.0)*angle;
        Quaternion<float> q(r,p, y);
        q.Normalize();
    
        // set the previous position
        // this will represent direction and speed when using verlet 
        // integration for updating position
        //         particle.previousPosition = particle.position - 
        //             (q.RotateVector(direction.RotateVector(Vector<3,float>(0.0,-1.0,0.0))
        //                             *RandomAttribute(speed,speedVar)));


        // set velocity and forces for use with euler integration
        particle.velocity = q.RotateVector(direction.RotateVector(Vector<3,float>(0.0,-1.0,0.0))
                                           *RandomAttribute(speed,speedVar));
        particle.forces = Vector<3,float>(0.0,0.0,0.0);
    }
    return emits;
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
    totalEmits = 0;
}

void AddTexture(ITextureResourcePtr texr) {
    inittex.AddTextureResource(texr);
}

TransformationNode* GetTransformationNode() {
    return transPos;
}

void SetTransformationNode(TransformationNode* node) {
    transPos = node;
}

};

}
}
#endif
