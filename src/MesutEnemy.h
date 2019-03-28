//
// Created by engin on 27.11.2017.
//

#ifndef LIMONENGINE_MESUTENEMY_H
#define LIMONENGINE_MESUTENEMY_H


#include "LimonAPI/ActorInterface.h"
#include "LimonAPI/LimonConverter.h"
#include <iostream>

class MesutEnemy: public ActorInterface {
    static ActorRegister<MesutEnemy> reg;

    const long PLAYER_SHOOT_TIMEOUT = 1000;
    long playerPursuitStartTime = 0L;
    long playerPursuitTimeout = 500000L; //if not see player for this amount, return.
    bool returnToPositionAfterPursuit = false;
    bool swingSoundStarted = false;
    glm::vec3 initialPosition;
    glm::vec3 lastWalkDirection;
    std::string currentAnimation;
    bool hitAnimationAwaiting = false;
    long dieAnimationStartTime = 0;
    long hitAnimationStartTime = 0;
    long lastSetupTime;
    long shootPlayerTimer = 0;
    uint32_t hitPoints = 100;

    std::vector<glm::vec3> routeTorequest;
    long routeGetTime = 0;
    bool routeRequested = false;

public:
    MesutEnemy(uint32_t id, LimonAPI *limonAPI) : ActorInterface(id, limonAPI) {}

    void play(long time, ActorInterface::ActorInformation &information) override;

    bool interaction(std::vector<LimonAPI::ParameterRequest> &interactionInformation) override;

    std::vector<LimonAPI::ParameterRequest> getParameters() const override;

    void setParameters(std::vector<LimonAPI::ParameterRequest> parameters) override;


    std::string getName() const override {
        return "MESUT_ENEMY";
    }
};




#endif //LIMONENGINE_MESUTENEMY_H
