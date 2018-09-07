#include "agent.h"

namespace Interface
{

class StateException;

Agent::Agent():
    resources_(0)
{

}
QString Agent::typeName() const{
    return "Agent";
}
QString Agent::name() const{
    return "";
}
QString Agent::title() const{
    return "Basic agent";
}
std::weak_ptr<Interface::Location> Agent::location() const{

    if (location_.lock() !=nullptr){
        return location_;
    }else{
        throw Interface::StateException(QString("Agent has no location"));
    }
}

std::weak_ptr<Interface::Player> Agent::owner() const{
    if (owner_.lock() != nullptr){
        return owner_;
    } else{
        throw Interface::StateException(QString("Agent has no owner"));
    }
}

void Agent::setOwner(std::weak_ptr<Interface::Player> owner){
    owner_ = owner;
}

bool Agent::isCommon() const{
    if (location_.lock() == nullptr and owner_.lock() == nullptr){
        return true;
    } else {
        return false;
    }
}

std::weak_ptr<Interface::Location> Agent::placement() const{
    return location_;
}
void Agent::setPlacement(std::weak_ptr<Interface::Location> placement){
    location_ = placement;
}

unsigned short Agent::connections() const{
    return location_.lock()->influence(owner_.lock());
}

void Agent::setConnections(unsigned short connections){
    location_.lock()->setInfluence(owner_.lock(),
                                   location_.lock()->influence(owner_.lock())+connections);
}

void Agent::modifyConnections(short amount){

    setConnections(amount+ int(floor(double(resources_)/5)));
}

void Agent::addResources(unsigned short amount){
    resources_ += amount;
}

int Agent::draws() const {
    return 1 + floor(resources_/6);
}
unsigned short Agent::resources() const{
    return resources_;
}
} //interface
