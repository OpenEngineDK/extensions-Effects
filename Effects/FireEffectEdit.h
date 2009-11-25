#ifndef _OEPARTICLE_FIRE_EFFECT_EDITOR_H_
#define _OEPARTICLE_FIRE_EFFECT_EDITOR_H_

#include <Effects/FireEffect.h>
#include <Widgets/Widgifier.h>

namespace OpenEngine {
    namespace Effects {
        
using namespace Widgets;

class FireEffectEdit : public FireEffect {
    WIDGET_INIT();
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
        WIDGET_PROPERTY(Play, GetActive, SetActive, FireEffectEdit, BUTTON_STATE);
        WIDGET_PROPERTY(number, GetNumber, SetNumber, FireEffectEdit, INT_VALUE);
        WIDGET_PROPERTY(speed, GetSpeed, SetSpeed, FireEffectEdit, FLOAT_VALUE);
        WIDGET_PROPERTY(spin,  GetSpin,  SetSpin,  FireEffectEdit, FLOAT_VALUE);
        WIDGET_PROPERTY(life,  GetLife,  SetLife,  FireEffectEdit, FLOAT_VALUE);
        WIDGET_PROPERTY(angle, GetAngle, SetAngle, FireEffectEdit, FLOAT_VALUE);
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

}
}
#endif
