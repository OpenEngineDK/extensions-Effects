#ifndef _OEPARTICLE_FIRE_EFFECT_EDITOR_H_
#define _OEPARTICLE_FIRE_EFFECT_EDITOR_H_

#include <Effects/FireEffect.h>
#include <Widgets/Widgifier.h>

namespace OpenEngine {
    namespace Effects {
        
using namespace Widgets;

class FireEffectEdit : public FireEffect {
    // WIDGET_INIT();
public:
    FireEffectEdit(OpenEngine::ParticleSystem::ParticleSystem& system,
                   unsigned int numParticles,
                   float emitRate,
                   float number, float numberVar,
                   float life, float lifeVar,
                   float angle, 
                   float spin, float spinVar,
                   float speed, float speedVar,
                   Vector<3,float> antigravity,
                   Renderers::TextureLoader& textureloader): 
    FireEffect(system
               , numParticles
               , emitRate
               , number
               , numberVar
               , life
               , lifeVar
               , angle
               , spin
               , spinVar
               , speed
               , speedVar
               , antigravity
               , textureloader) 
    {
    }
    
    virtual ~FireEffectEdit() {
    }
    
    float GetSpeed() {
        return speed;
    }
    
    float GetSpin() {
        return spin;
    }
    
    float GetLife() {
        return life;
    }
    
    float GetAngle() {
        return 180 * angle / Math::PI;
    }

    int GetNumber() {
        return number;
    }
    
    void SetSpeed(float speed) {
        this->speed = speed;
    }
    
    void SetSpin(float spin) {
        this->spin = spin;
    }
    
    void SetLife(float life) {
        this->life = life;
    }
    
    void SetAngle(float angle) {
        this->angle = Math::PI * angle / 180.0;
    }

    void SetNumber(float number) {
        this->number = number;
    }

};

#define _STEP 1.5
WIDGET_START(FireEffectEdit, FireEffectEditWidget);
        WIDGET_BUTTON("Play", GetActive, SetActive, TOGGLE);
        WIDGET_CSLIDER("Number", GetNumber, SetNumber, int, 1);
        WIDGET_CSLIDER("Speed", GetSpeed, SetSpeed, float, _STEP);
        WIDGET_CSLIDER("Spin",  GetSpin,  SetSpin, float, _STEP);
        WIDGET_CSLIDER("Life",  GetLife,  SetLife, float, _STEP);
        WIDGET_CSLIDER("Angle", GetAngle, SetAngle, float, _STEP);
WIDGET_STOP();

}
}
#endif
