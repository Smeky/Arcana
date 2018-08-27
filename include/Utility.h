#ifndef UTILITY_H
#define UTILITY_H

#include "CGame.h"
#include "typeinfo"

struct lua_State;

//struct Polygon {
//    std::vector<sf::Vector2f> p;
//};

namespace Util {

float           degreeToRad     ( const float degree );
float           radToDegree     ( const float radian );
float           angleByTarget   ( const sf::Vector2f src, const sf::Vector2f target );

float           lenghtByVectors ( const sf::Vector2f a, const sf::Vector2f b );
float           lenghtyByVel    ( const sf::Vector2f velocity );
sf::Vector2f    normalize       ( const sf::Vector2f vec );
float           dotProduct      ( const sf::Vector2f a, const sf::Vector2f b );
float           angleDotProduct ( const sf::Vector2f a, const sf::Vector2f b );

sf::Vector2f    moveByTarget    ( const sf::Vector2f src, const sf::Vector2f dest, const float speed );
sf::Vector2f    moveByAngle     ( const float angleRad, const float speed );

bool            collisionSquare ( const sf::FloatRect a, const sf::FloatRect b );
bool            collCircleToRect( const Circle& circle, const sf::FloatRect& rectangle );
bool            collLineToLine  ( const sf::Vector2f a1, const sf::Vector2f a2, const sf::Vector2f b1, const sf::Vector2f b2 );
//    bool            collPolygToRect ( const Polygon polygon, const sf::FloatRect rect );

bool            isRectInside    ( const sf::FloatRect& a, const sf::FloatRect& b );

//// Check if rectangle a is inside b ( not intersecting )
//template<typename A, typename B>
//bool            isRectInside    ( const sf::Rect<A>& a, const sf::Rect<B>& b ) {
//         if( a.left >= b.left + b.width )   return false;
//    else if( a.left + a.width <= b.left )   return false;
//    else if( a.top >= b.top + b.height )    return false;
//    else if( a.top + a.height <= b.top )    return false;
//
//    return true;
//}

// Change type of the sf::Vector2
template<typename New, typename Old>
sf::Vector2<New>convertVector   ( const sf::Vector2<Old>& vec ) {
    // If converting to integer, round properly
    if( typeid( New ) == typeid( int ) ) {
        return sf::Vector2<New>( vec.x + 0.5, vec.y + 0.5 );
    }
    else {
        return sf::Vector2<New>( vec.x, vec.y );
    }
}

// Change type of the sf::Rect
template<typename New, typename Old>
sf::Rect<New>   convertRect     ( const sf::Rect<Old>& rect ) {
    // If converting to integer, round properly
    if( typeid( New ) == typeid( int ) ) {
        return sf::Rect<New>( rect.left + 0.5, rect.top + 0.5, rect.width + 0.5, rect.height + 0.5 );
    }
    else {
        return sf::Rect<New>( rect.left, rect.top, rect.width, rect.height );
    }
}

template<typename Result, typename Left, typename Right >
sf::Vector2<Result> multiplyVectors ( const sf::Vector2<Left>& left, const sf::Vector2<Right>& right ) {
    return sf::Vector2<Result>( left.x * right.x, left.y * right.y );
}

template<typename Result, typename Left, typename Right >
sf::Vector2<Result> divideVectors ( const sf::Vector2<Left>& left, const sf::Vector2<Right>& right ) {
    return sf::Vector2<Result>( left.x / right.x, left.y / right.y );
}

sf::Vector2f    rectPosToVec    ( const sf::FloatRect rect );
sf::Vector2f    rectSizeToVec   ( const sf::FloatRect rect );

sf::Vector2f    centerPos       ( const sf::Vector2f pos, const sf::Vector2f size );
sf::Vector2f    centerOrigin    ( const sf::FloatRect globalBounds );
sf::Vector2f    centerOriginInt ( const sf::FloatRect& bounds );
sf::Vector2f    boxCenterPos    ( const sf::FloatRect& box );

// Resize sprite based on clip size and new size
template<typename T>
void            resizeSprite    ( sf::Sprite& sprite, const sf::Vector2<T>& size ) {
    sf::IntRect  clip       = sprite.getTextureRect();

    sprite.setScale( (float)size.x / clip.width, (float)size.y / clip.height );
}

// Resize sprite based on clip size and new size
template<typename T>
void            resizeSprite    ( sf::Sprite& sprite, const sf::Rect<T>& size ) {
    sf::IntRect  clip       = sprite.getTextureRect();

    sprite.setScale( (float)size.width / clip.width, (float)size.height / clip.height );
}

std::string     intToString     ( int num );
std::string     uintToString    ( size_t num );
std::string     floatToString   ( float num );
std::string     decimalToString ( float num, size_t decimals = 1 );

template<typename T>
sf::Vector2<T> vectorFromTable( lua_State* state, int index, const char* field = nullptr ) {
    sf::Vector2<T> result;

    if( field != nullptr ) {
        lua_getfield( state, index, field );

        index = lua_gettop( state );
    }

    if( lua_istable( state, index ) ) {
        lua_getfield( state, index, "x" );
        lua_getfield( state, index, "y" );

        // Check if values are based on key or index
        // Index based
        if( lua_isnil( state, - 1 ) || lua_isnil( state, - 2 ) ) {
            lua_rawgeti( state, index, 1 );
            lua_rawgeti( state, index, 2 );

            result.x = (T)lua_tonumber( state, - 2 );
            result.y = (T)lua_tonumber( state, - 1 );

            lua_pop( state, 2 );
        }
        // Key based
        else {
            result.x = (T)lua_tonumber( state, - 2 );
            result.y = (T)lua_tonumber( state, - 1 );
        }

        lua_pop( state, 2 );
    }

    if( field != nullptr ) {
        lua_pop( state, 1 );
    }

    return result;
}

sf::Vector2f    vector2fFromLuaTable    ( lua_State* state, int index );
sf::FloatRect   floatrectFromLuaTable   ( lua_State* state, int index );
sf::IntRect     intrectFromLuaTable     ( lua_State* state, int index );
sf::Color       colorFromLuaTable       ( lua_State* state, int index );
Circle          circleFromLuaTable      ( lua_State* state, int index );

CTextWrapper    textWrapperFromLuaTable ( lua_State* state, int index );

} // namespace Util


#endif // UTILITY_H
