#include "Car.hpp"
#include "util/VectorMath.hpp"

#include <iostream>
#include <algorithm> // std::max
#include "Rando.hpp"

namespace ts
{

std::vector<const sf::Texture *> Car::Textures_;
unsigned int Car::accident_count_ = 0;

Car::Car(const std::shared_ptr<Node> &pos, const std::shared_ptr<Node> &dest, const sf::Vector2f &size)
    : pos_(pos), dest_(dest), prev_(pos), shape_(size), speed_(200), acceleration_(200)
{
    Rando r(Textures_.size());
    int r_i = r.uniroll();
    shape_.setTexture(Textures_.at(r_i - 1));
    shape_.setOrigin({shape_.getSize().x / 2, shape_.getSize().y / 2});
    shape_.setPosition(pos_->getPos());
    findRoute();
}

void Car::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    target.draw(shape_, states);
}

void Car::AddTexture(const sf::Texture *carTexture)
{
    Textures_.push_back(carTexture);
}

void Car::update(const sf::Time &game_time, float deltatime, const std::vector<std::unique_ptr<Car>> &cars, const std::map<unsigned int, std::unique_ptr<TrafficLightNetwork>> &light_handlers)
{
    if (route_.size() < 1)
    {
        finished = true;
        return;
    }

    // Prüfe auf mögliche Unfälle
    if (!is_in_accident_ && checkAccident(cars))
    {
        handleAccident();
    }

    float delta_step = deltatime * speed_;
    if (VectorMath::Distance(shape_.getPosition(), route_.front()->getPos()) < delta_step)
    {
        // Increment node cars_passed counter
        prev_->incrementCounter(game_time);
        shape_.setPosition(route_.front()->getPos());
        prev_ = route_.front();
        route_.pop_front();
        if (route_.size() < 1 || VectorMath::Distance(dest_->getPos(), route_.front()->getPos()) < delta_step)
        {
            finished = true;
            return;
        }
        // We only need to change rotation of the car on turn
        dir_ = VectorMath::Normalize(route_.front()->getPos() - prev_->getPos());
        if (dir_.x == 1)
            shape_.setRotation(90);
        else if (dir_.x == -1)
            shape_.setRotation(270);
        else if (dir_.y == -1)
            shape_.setRotation(0);
        else if (dir_.y == 1)
            shape_.setRotation(180);
    }
    // Checks if there is something (another car) infront of this car
    // if there is -> we cant move forward
    calculateVelocity(deltatime, cars, light_handlers);
    shape_.move(dir_ * deltatime * speed_);
}

void Car::calculateVelocity(float deltatime, const std::vector<std::unique_ptr<Car>> &cars, const std::map<unsigned int, std::unique_ptr<TrafficLightNetwork>> &light_handlers)
{
    for (const auto &car : cars)
    {
        // safe distance to the car infront
        if (car->shape_.getGlobalBounds().contains(shape_.getPosition() + dir_ * shape_.getSize().y))
        {
            speed_ = 0;
            return;
        }
        // car front
        if (car->shape_.getGlobalBounds().contains(shape_.getPosition() + dir_ * shape_.getSize().y * 0.51f))
        {
            speed_ = 0;
            return;
        }
    }
    for (auto ita = light_handlers.begin(); ita != light_handlers.end(); ++ita)
    {
        const auto &lights = ita->second->getLights();
        //sf::FloatRect rect(shape_.getPosition() - shape_.getOrigin(), {std::max(dir_.x * shape_.getSize().y*5.f, 2.f), std::max(dir_.y * shape_.getSize().y*5.f, 2.f)});
        for (const auto &light : lights)
        {
            if (!light->canDrive())
            {
                // if (light->getBlocker().getGlobalBounds().intersects(rect))
                // {
                //     speed_ = std::max(speed_ - acceleration_ * acceleration_ * deltatime, 0.f);
                //     return;
                // }
                // // car front
                // if (light->getBlocker().getGlobalBounds().intersects(rect))
                // {
                //     speed_ = std::max(speed_ - acceleration_ * acceleration_* deltatime, 0.f);
                //     return;
                // }
                if (light->getBlocker().getGlobalBounds().contains(shape_.getPosition() + dir_ * shape_.getSize().y))
                {
                    speed_ = 0;
                    return;
                }
                // car front
                if (light->getBlocker().getGlobalBounds().contains(shape_.getPosition() + dir_ * shape_.getSize().y * 0.51f))
                {
                    speed_ = 0;
                    return;
                }
            }
            // light infront
        }
    }
    speed_ = std::min(speed_ + acceleration_ * deltatime, 200.f);
}

void Car::findRoute()
{
    std::map<std::shared_ptr<Node>, bool> visited;
    Node::search_AStar(pos_, dest_, visited, route_);
}

bool Car::checkAccident(const std::vector<std::unique_ptr<Car>> &cars)
{
    const float MIN_SAFE_DISTANCE = shape_.getSize().y * 0.1f; // 10% der Fahrzeuglänge als Mindestabstand
    
    for (const auto &car : cars)
    {
        if (car.get() == this) continue; // Skip own car
        
        float distance = VectorMath::Distance(shape_.getPosition(), car->shape_.getPosition());
        if (distance < MIN_SAFE_DISTANCE)
        {
            // Check if the cars are driving in the same direction
            if (VectorMath::Dot(dir_, car->dir_) > 0.8f) // If the direction vectors are almost parallel
            {
                return true;
            }
        }
    }
    return false;
}

void Car::handleAccident()
{
    is_in_accident_ = true;
    speed_ = 0;
    accident_count_++; // Increase accident counter
    
    // Block the current road section
    if (prev_ && route_.size() > 0)
    {
        // Mark the current road section as blocked
        prev_->setBlocked(true);
        route_.front()->setBlocked(true);
        
        // Search for alternative route
        findRoute();
    }
}
} // namespace ts
