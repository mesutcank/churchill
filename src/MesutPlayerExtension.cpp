//
// Created by engin on 16.11.2018.
//

#include "MesutPlayerExtension.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/ext.hpp>

PlayerExtensionRegister<MesutPlayerExtension> MesutPlayerExtension::reg("MesutPlayerExtension");

const glm::quat MesutPlayerExtension::direction = glm::quat(0.0f, 0.0f, 1.0f, 0.0f);//this is used to reverse hit normal


void MesutPlayerExtension::swordSlash(const std::vector<LimonAPI::ParameterRequest> &parameterRequest [[gnu::unused]]) {
    limonAPI->playSound("./Data/Sounds/swordswish.wav", glm::vec3(0,0,0), false);
    std::vector<LimonAPI::ParameterRequest>rayResult = limonAPI->rayCastToCursor();
    if(rayResult.size() > 0 ) {

        //means we hit something
        if(rayResult.size() == 4) {
            LimonAPI::Vec4 _aiCoords = rayResult[1].value.vectorValue;
            std::vector<LimonAPI::ParameterRequest> playerCoordsLimon = limonAPI->getObjectTransformation(playerAttachedModelID);
            if (playerCoordsLimon.size() > 0) {
                glm::vec3 pCoords;
                pCoords.x = playerCoordsLimon[0].value.vectorValue.x;
                pCoords.y = playerCoordsLimon[0].value.vectorValue.y;
                pCoords.z = playerCoordsLimon[0].value.vectorValue.z;

                glm::vec3 aiCoords;
                aiCoords.x = _aiCoords.x;
                aiCoords.y = _aiCoords.y;
                aiCoords.z = _aiCoords.z;


                float distance = glm::distance(pCoords, aiCoords);
                std::cout << "distance " << distance << std::endl;;
                if (distance < 6) {
                    std::cout << "hit AI" << std::endl;
                    // means we hit something with AI, handle AI interaction
                    LimonAPI::ParameterRequest parameterRequest;
                    parameterRequest.valueType = LimonAPI::ParameterRequest::ValueTypes::STRING;
                    strncpy(parameterRequest.value.stringValue, "GOT_HIT", 63);
                    std::vector<LimonAPI::ParameterRequest> prList;
                    prList.push_back(parameterRequest);
                    limonAPI->interactWithAI(rayResult[3].value.longValue, prList);
                }

            }

        } else {
            std::cout << "hit non AI" << std::endl;
            //means we hit something that doesn't have AI, put a hole
            glm::vec3 scale(0.2f, 0.2f, 0.2f);
            glm::vec3 hitPos(rayResult[1].value.vectorValue.x, rayResult[1].value.vectorValue.y, rayResult[1].value.vectorValue.z);
            glm::vec3 hitNormal(rayResult[2].value.vectorValue.x, rayResult[2].value.vectorValue.y, rayResult[2].value.vectorValue.z);

            hitPos +=hitNormal * 0.002f; //move hit position a bit towards the normal to prevent zfight
            glm::quat orientation;
            if(hitNormal.x < 0.001 && hitNormal.x > -0.001 &&
               hitNormal.y < 1.001 && hitNormal.y >  0.999 &&
               hitNormal.z < 0.001 && hitNormal.z > -0.001) {
                //means the normal is up
                orientation = glm::quat(0.707f, -0.707f, 0.0f, 0.0f);
            } else {
#if (GLM_VERSION_MAJOR !=0 || GLM_VERSION_MINOR > 9 || (GLM_VERSION_MINOR == 9 && GLM_VERSION_PATCH >= 9))
                orientation =  glm::quatLookAt(hitNormal * -1.0f, glm::vec3(0,1,0));
#else
                glm::mat3 Result;

                        Result[2] = -1.0f * hitNormal;//this -1 shouldn't be needed
                        Result[0] = glm::normalize(cross(glm::vec3(0,1,0), Result[2]));
                        Result[1] = glm::cross(Result[2], Result[0]);

                        orientation = glm::quat_cast(Result);
#endif
            }


            std::vector<LimonAPI::ParameterRequest>modelTransformationMat = limonAPI->getObjectTransformationMatrix(rayResult[0].value.longValue);

        }

    } else {
        std::cout << "hit nothing!" << std::endl;
    }
}

void MesutPlayerExtension::processInput(const InputStates &inputState, const PlayerExtensionInterface::PlayerInformation &playerInformation, long time [[gnu::unused]]) {
    if (playerAttachedModelID == 0) {
        return;
    }

    const bool *keyStates = inputState.getRawKeyStates();

    if (keyStates['r']) {
        // can not remove self
        //limonAPI->loadAndSwitchWorld("./Data/Maps/WorldMesut.xml");
        return;
    }


    if(inputState.getInputEvents(InputStates::Inputs::MOUSE_BUTTON_LEFT) && inputState.getInputStatus(InputStates::Inputs::MOUSE_BUTTON_LEFT)) {
        if((limonAPI->getModelAnimationName(playerAttachedModelID) != "Great Sword Slash|" ||  limonAPI->getModelAnimationFinished(playerAttachedModelID))) {
            limonAPI->setModelAnimation(playerAttachedModelID, "Great Sword Slash|", false);
            limonAPI->addTimedEvent(250, std::bind(&MesutPlayerExtension::swordSlash, this, std::placeholders::_1), std::vector<LimonAPI::ParameterRequest>());

            glm::vec3 scale(10.0f,10.0f,10.0f);//it is actually 0,1 * 1/baseScale

            //now put bullet hole to the hit position:


        }
    } else {
        if (inputState.getInputEvents(InputStates::Inputs::MOUSE_BUTTON_RIGHT)) {
            LimonAPI::Vec4 newOffset = LimonAPI::Vec4(0.075f, 0.03f,-0.045f);
            LimonAPI::Vec4 attachedModelOffset = limonAPI->getPlayerAttachedModelOffset();
            if(inputState.getInputStatus(InputStates::Inputs::MOUSE_BUTTON_RIGHT)) {
                limonAPI->setModelAnimationWithBlend(playerAttachedModelID, "AimPose", true);

                attachedModelOffset = attachedModelOffset + newOffset;
            } else {
                limonAPI->setModelAnimationWithBlend(playerAttachedModelID, "AimPose", false);
                attachedModelOffset = attachedModelOffset - newOffset;
            }
            limonAPI->setPlayerAttachedModelOffset(attachedModelOffset);
        }

        bool isAnimationFinished = limonAPI->getModelAnimationFinished(playerAttachedModelID);
        std::string currentAnimationName = limonAPI->getModelAnimationName(playerAttachedModelID);

        if(!inputState.getInputStatus(InputStates::Inputs::MOVE_FORWARD) &&
           !inputState.getInputStatus(InputStates::Inputs::MOVE_BACKWARD) &&
           !inputState.getInputStatus(InputStates::Inputs::MOVE_LEFT) &&
           !inputState.getInputStatus(InputStates::Inputs::MOVE_RIGHT)) {
            //standing still
            std::string finishedStr = " not finished";


            if(isAnimationFinished) {
                finishedStr = " finished";
            }

            if((currentAnimationName == "Run" ||
                currentAnimationName == "Walk") ||
               isAnimationFinished) {
                limonAPI->setModelAnimationWithBlend(playerAttachedModelID, "Great Sword Idle|", true);
            }
        } else {
            //we are moving. Set only if we just started.
            if(currentAnimationName == "Run" ||
               currentAnimationName == "Walk") {
                //we were already moving, handle if player run state changed
                if (inputState.getInputEvents(InputStates::Inputs::RUN)) {
                    if (inputState.getInputStatus(InputStates::Inputs::RUN)) {
                        limonAPI->setModelAnimationWithBlend(playerAttachedModelID, "Great Sword Run|", true);
                    } else {
                        limonAPI->setModelAnimationWithBlend(playerAttachedModelID, "Great Sword Run|", true);
                    }
                }
            } else {
                if(currentAnimationName == "Idle" ||
                   isAnimationFinished) {
                    //we were standing or some other animation. handle accordingly
                    if (inputState.getInputStatus(InputStates::Inputs::RUN)) {
                        limonAPI->setModelAnimationWithBlend(playerAttachedModelID, "Great Sword Run|", true);
                    } else {
                        limonAPI->setModelAnimationWithBlend(playerAttachedModelID, "Great Sword Run|", true);
                    }
                }
            }
        }

    }

}

void MesutPlayerExtension::removeDamageIndicator(std::vector<LimonAPI::ParameterRequest> parameters) {
    if(parameters.size() > 0 && parameters[0].valueType == LimonAPI::ParameterRequest::ValueTypes::LONG) {
        limonAPI->removeGuiElement(parameters[0].value.longValue);
    }
}

void MesutPlayerExtension::interact(std::vector<LimonAPI::ParameterRequest> &interactionData) {
    static uint32_t removeCounter = 0;
    static uint32_t addedElement = 0;

    if(interactionData.size() == 0 ) {
        return;
    }

    if(interactionData[0].valueType == LimonAPI::ParameterRequest::ValueTypes::STRING && std::string(interactionData[0].value.stringValue) == "SHOOT_PLAYER") {
        hitPoints -= 5;

        limonAPI->updateGuiText(40, std::to_string(hitPoints));
        if(hitPoints <= 0) {
            limonAPI->killPlayer();
        }

        if(addedElement == 0) {
            addedElement = limonAPI->addGuiImage("./Data/Textures/mesutDamageIndicator.png", "damageIndicator", LimonAPI::Vec2(0.5f, 0.5f),
                                                 LimonAPI::Vec2(0.7f, 0.8f), 0);

        }
        std::vector<LimonAPI::ParameterRequest> removeParameters;
        LimonAPI::ParameterRequest removeID;
        removeID.valueType = LimonAPI::ParameterRequest::ValueTypes::LONG;
        removeID.value.longValue = addedElement;
        removeParameters.push_back(removeID);
        limonAPI->addTimedEvent(250, std::bind(&MesutPlayerExtension::removeDamageIndicator, this, std::placeholders::_1), removeParameters);
    }

    if(addedElement !=0) {
        if(removeCounter == 0) {
            //limonAPI->removeGuiElement(addedElement);
            addedElement = 0;
        } else {
            removeCounter--;
        }
    }


}

std::string MesutPlayerExtension::getName() const {
    return "ShooterPlayerExtension";
}



extern "C" void registerPlayerExtensions(std::map<std::string, PlayerExtensionInterface*(*)(LimonAPI*)>* playerExtensionMap) {
    (*playerExtensionMap)["MesutPlayerExtension"] = &createPlayerExtension<MesutPlayerExtension>;

}