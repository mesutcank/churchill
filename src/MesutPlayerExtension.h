//
// Created by engin on 16.11.2018.
//

#ifndef LIMONENGINE_MESUTPLAYEREXTENSION_H
#define LIMONENGINE_MESUTERPLAYEREXTENSION_H


#include "LimonAPI/PlayerExtensionInterface.h"
#include "LimonAPI/LimonConverter.h"
#include <iostream>


class MesutPlayerExtension : public  PlayerExtensionInterface {
    static PlayerExtensionRegister<MesutPlayerExtension> reg;

    static const glm::quat direction;
    glm::vec3 muzzleFlashOffset = glm::vec3(-0.18f,2.85f,0.5750f);
    uint32_t playerAttachedModelID;

    int hitPoints = 100;
public:

    MesutPlayerExtension(LimonAPI* limonAPI) : PlayerExtensionInterface(limonAPI) {
        playerAttachedModelID = limonAPI->getPlayerAttachedModel();
    }
    void removeDamageIndicator(std::vector<LimonAPI::ParameterRequest> parameters);
    void processInput(const InputStates &inputState, const PlayerExtensionInterface::PlayerInformation &playerInformation, long time) override;

    void interact(std::vector<LimonAPI::ParameterRequest> &interactionData) override;

    std::string getName() const override;
    void swordSlash(const std::vector<LimonAPI::ParameterRequest> &parameterRequest);

};



#endif //LIMONENGINE_SHOOTERPLAYEREXTENSION_H
