#include "Utility.h"

#include "sstream"
#include "math.h"
#include "cstdio"
#include "lua.hpp"

#define PI 3.1415926535897

#include "iostream"
using namespace std;

// Convert degrees to radians
float Util::degreeToRad( const float degree ) {
    return degree * ( PI / 180 );
}

// Convert radians to degrees
float Util::radToDegree( const float radian ) {
    return radian * ( 180 / PI );
}

// Calculate angle by target destination
float Util::angleByTarget( const sf::Vector2f src, const sf::Vector2f target ) {
    return atan2f( target.y - src.y, target.x - src.x );
}

float Util::lenghtByVectors( const sf::Vector2f a, const sf::Vector2f b ) {
    sf::Vector2f x = a - b;

    return sqrtf( x.x * x.x + x.y * x.y );
}

float Util::lenghtyByVel( const sf::Vector2f velocity ) {
    return sqrt( velocity.x * velocity.x + velocity.y * velocity.y );
}

sf::Vector2f Util::normalize( const sf::Vector2f vec ) {
    return vec / lenghtyByVel( vec );
}

float Util::dotProduct( const sf::Vector2f a, const sf::Vector2f b ) {
    return ( a.x * b.x ) + ( a.y * b.y );
}

float Util::angleDotProduct( const sf::Vector2f a, const sf::Vector2f b ) {
    return acosf( ( a.x * b.x ) + ( a.y * b.y ) );
}

// Calculate new position based on target destination and speed
sf::Vector2f Util::moveByTarget ( const sf::Vector2f src, const sf::Vector2f dest, const float speed ) {
    sf::Vector2f movePos = src;

    float angle = angleByTarget( src, dest );

    movePos.x += speed * cosf( angle );
    movePos.y += speed * sinf( angle );

    float destin = lenghtByVectors( src, dest );
    float moveme = lenghtByVectors( src, movePos );

    if( destin < moveme ) {
        movePos = dest;
    }

    return movePos;
}

// Calculate new movement velocity based on angle and speed
sf::Vector2f Util::moveByAngle ( const float angleRad, const float speed ) {
    sf::Vector2f movePos;
    movePos.x = speed * cosf( angleRad );
    movePos.y = speed * sinf( angleRad );

    return movePos;
}

bool Util::collisionSquare( const sf::FloatRect a, const sf::FloatRect b ) {
    return a.intersects( b );
}

bool Util::collCircleToRect( const Circle& c, const sf::FloatRect& r ) {
    float cx    = fabs( c.x - r.left - r.width / 2 );
    float xDist = r.width / 2 + c.r;
    if( cx > xDist ) { return false; }

    float cy    = fabs( c.y - r.top - r.height / 2 );
    float yDist = r.height / 2 + c.r;
    if( cy > yDist ) { return false; }

    if( cx <= r.width / 2 ) { return true; }
    if( cy <= r.height / 2 ) { return true; }

    float xCornerDist   = cx - r.width / 2;
    float yCornerDist   = cy - r.height / 2;
    float xCornerDistSq = xCornerDist * xCornerDist;
    float yCornerDistSq = yCornerDist * yCornerDist;
    float maxCornerDistSq = c.r * c.r;

    return xCornerDistSq + yCornerDistSq <= maxCornerDistSq;
}

bool Util::collLineToLine( const sf::Vector2f a1, const sf::Vector2f a2, const sf::Vector2f b1, const sf::Vector2f b2 ) {
    sf::Vector2f s1;
    sf::Vector2f s2;

    s1.x = a2.x - a1.x;
    s1.y = a2.y - a1.y;

    s2.x = b2.x - b1.x;
    s2.y = b2.y - b1.y;

    float s = ( -s1.y * ( a1.x - b1.x ) + s1.x * ( a1.y - b1.y ) ) / ( -s2.x * s1.y + s1.x * s2.y );
    float t = ( s2.x * ( a1.y - b1.y ) - s2.y * ( a1.x - b1.x ) ) / ( -s2.x * s1.y + s1.x * s2.y );

    if( s >= 0 && s <= 1 && t >= 0 && t <= 1 ) {
        return true;
    }

    return false;
}
//
//bool Util::collPolygToRect( const Polygon polygon, const sf::FloatRect rect ) {
//    sf::Vector2f rp1( rect.left, rect.top );
//    sf::Vector2f rp2( rect.left + rect.width, rect.top );
//    sf::Vector2f rp3( rect.left + rect.width, rect.top + rect.height );
//    sf::Vector2f rp4( rect.left, rect.top + rect.height );
//
//    for( unsigned int i = 1; i < polygon.p.size(); i++ ) {
//             if( collLineToLine( polygon.p[ i - 1 ], polygon.p[ i ], rp1, rp2 ) ) { return true; }
//        else if( collLineToLine( polygon.p[ i - 1 ], polygon.p[ i ], rp2, rp3 ) ) { return true; }
//        else if( collLineToLine( polygon.p[ i - 1 ], polygon.p[ i ], rp3, rp4 ) ) { return true; }
//        else if( collLineToLine( polygon.p[ i - 1 ], polygon.p[ i ], rp4, rp1 ) ) { return true; }
//    }
//
//    return false;
//}

 bool Util::isRectInside( const sf::FloatRect& a, const sf::FloatRect& b ) {
    if( a.left      >= b.left &&
        a.top       >= b.top  &&
        a.left  + a.width   <= b.left   + b.width &&
        a.top   + a.height  <= b.top    + b.height )
    {
        return true;
    }

    return false;
}

sf::Vector2f Util::rectPosToVec( const sf::FloatRect rect ) {
    return sf::Vector2f( rect.left, rect.top );
}

sf::Vector2f Util::rectSizeToVec( const sf::FloatRect rect ) {
    return sf::Vector2f( rect.width, rect.height );
}

sf::Vector2f Util::centerOrigin( const sf::FloatRect globalBounds ) {
    return sf::Vector2f( globalBounds.width / 2, globalBounds.height / 2 );
}

sf::Vector2f Util::centerOriginInt( const sf::FloatRect& bounds ) {
    return sf::Vector2f( (int)( bounds.width / 2 + 0.5 ), (int)( bounds.height / 2 + 0.5 ) );
}

sf::Vector2f Util::boxCenterPos( const sf::FloatRect& box ) {
    return sf::Vector2f( box.left + box.width / 2, box.top + box.height / 2 );
}

std::string Util::intToString( int num ) {
    std::ostringstream oss;
    oss << num;
    return oss.str();
}

std::string Util::uintToString( size_t num ) {
    std::ostringstream oss;
    oss << num;
    return oss.str();
}

std::string Util::floatToString( float num ) {
    char buffer[ 2 ];

    snprintf( buffer, 2, "%f", num );

    return std::string( buffer );
}

std::string Util::decimalToString( float num, size_t decimals ) {
    char buffer[ decimals + 3 ];

    snprintf( buffer, decimals + 3, "%f", num );

    return std::string( buffer );
}

sf::Vector2f Util::vector2fFromLuaTable( lua_State* state, int index ) {
    sf::Vector2f result;

    if( lua_istable( state, index ) ) {
        lua_rawgeti( state, index, 1 );
        lua_rawgeti( state, index, 2 );

        if( lua_isnil( state, - 1 ) || lua_isnil( state, - 2 ) ) {
            cout << "Warning: Wrong amount of arguments in vector2f" << endl;
        }

        if( lua_isnumber( state, - 2 ) ) {
            result.x    = lua_tonumber( state, - 2 );
        }

        if( lua_isnumber( state, - 1 ) ) {
            result.y    = lua_tonumber( state, - 1 );
        }

        lua_pop( state, 2 );
    }
    else {
        cout << "Warning: Vector2f can't be made out of non-table value" << endl;
    }

    return result;
}

sf::FloatRect Util::floatrectFromLuaTable( lua_State* state, int index ) {
    sf::FloatRect result;

    if( lua_istable( state, index ) ) {
        lua_rawgeti( state, index, 1 );
        lua_rawgeti( state, index, 2 );
        lua_rawgeti( state, index, 3 );
        lua_rawgeti( state, index, 4 );

        for( int i = 1; i != 4; i++ ) {
            if( lua_isnil( state, - i ) ) {
                cout << "Warning: Wrong amount of arguments in floatrect" << endl;

                break;
            }
        }

        if( lua_isnumber( state, - 4 ) ) {
            result.left     = lua_tonumber( state, - 4 );
        }

        if( lua_isnumber( state, - 3 ) ) {
            result.top      = lua_tonumber( state, - 3 );
        }

        if( lua_isnumber( state, - 2 ) ) {
            result.width    = lua_tonumber( state, - 2 );
        }

        if( lua_isnumber( state, - 1 ) ) {
            result.height   = lua_tonumber( state, - 1 );
        }

        lua_pop( state, 4 );
    }
    else {
        cout << "Warning: FloatRect can't be made out of non-table value" << endl;
    }

    return result;
}

sf::IntRect Util::intrectFromLuaTable( lua_State* state, int index ) {
    sf::IntRect result;

    if( lua_istable( state, index ) ) {
        lua_rawgeti( state, index, 1 );
        lua_rawgeti( state, index, 2 );
        lua_rawgeti( state, index, 3 );
        lua_rawgeti( state, index, 4 );

        for( int i = 1; i != 4; i++ ) {
            if( lua_isnil( state, - i ) ) {
                cout << "Warning: Wrong amount of arguments in intrect - " << i << endl;

                break;
            }
        }

        if( lua_isnumber( state, - 4 ) ) {
            result.left     = lua_tointeger( state, - 4 );
        }

        if( lua_isnumber( state, - 3 ) ) {
            result.top      = lua_tointeger( state, - 3 );
        }

        if( lua_isnumber( state, - 2 ) ) {
            result.width    = lua_tointeger( state, - 2 );
        }

        if( lua_isnumber( state, - 1 ) ) {
            result.height   = lua_tointeger( state, - 1 );
        }

        lua_pop( state, 4 );
    }
    else {
        cout << "Warning: IntRect can't be made out of non-table value" << endl;
    }

    return result;
}

sf::Color Util::colorFromLuaTable( lua_State* state, int index ) {
    sf::Color result;

    if( lua_istable( state, index ) ) {
        lua_rawgeti( state, index, 1 );
        lua_rawgeti( state, index, 2 );
        lua_rawgeti( state, index, 3 );
        lua_rawgeti( state, index, 4 );

        for( int i = 2; i != 4; i++ ) {
            if( lua_isnil( state, - i ) ) {
                cout << "Warning: Wrong amount of arguments in color - " << i << endl;

                break;
            }
        }

        if( lua_isinteger( state, - 4 ) ) {
            result.r    = lua_tointeger( state, - 4 );
        }

        if( lua_isinteger( state, - 3 ) ) {
            result.g    = lua_tointeger( state, - 3 );
        }

        if( lua_isinteger( state, - 2 ) ) {
            result.b    = lua_tointeger( state, - 2 );
        }

        if( lua_isinteger( state, - 1 ) ) {
            result.a    = lua_tointeger( state, - 1 );
        }

        lua_pop( state, 4 );
    }
    else {
        cout << "Warning: Color can't be made out of non-table value" << endl;
    }



    return result;
}

Circle Util::circleFromLuaTable( lua_State* state, int index ) {
    Circle result;

    if( lua_istable( state, index ) ) {
        lua_rawgeti( state, index, 1 );
        lua_rawgeti( state, index, 2 );
        lua_rawgeti( state, index, 3 );

        for( int i = 1; i != 3; i++ ) {
            if( lua_isnil( state, - i ) ) {
                cout << "Warning: Wrong amount of arguments in circle - " << i << endl;

                break;
            }
        }

        if( lua_isnumber( state, - 3 ) ) {
            result.x    = lua_tonumber( state, - 3 );
        }

        if( lua_isnumber( state, - 2 ) ) {
            result.y    = lua_tonumber( state, - 2 );
        }

        if( lua_isnumber( state, - 1 ) ) {
            result.r    = lua_tonumber( state, - 1 );
        }

        lua_pop( state, 3 );
    }
    else {
        cout << "Warning: Circle can't be made out of non-table value" << endl;
    }

    return result;
}

CTextWrapper Util::textWrapperFromLuaTable( lua_State* state, int index ) {
    CTextWrapper text;

    if( lua_istable( state, index ) ) {

        lua_getfield( state, index, "pos" );
        if( !lua_isnil( state, - 1 ) ) {
            text.setPos( Util::vector2fFromLuaTable( state, lua_gettop( state ) ) );
        }

        lua_getfield( state, index, "font" );
        if( !lua_isnil( state, - 1 ) ) {
            text.setFont( lua_tostring( state, - 1 ) );
        }

        lua_getfield( state, index, "size" );
        if( lua_isinteger( state, - 1 ) ) {
            text.setCharSize( lua_tointeger( state, - 1 ) );
        }

        lua_getfield( state, index, "style" );
        if( lua_isinteger( state, - 1 ) ) {
            text.setStyle( (sf::Text::Style)lua_tointeger( state, - 1 ) );
        }

        lua_getfield( state, index, "color" );
        if( !lua_isnil( state, - 1 ) ) {
            text.setColor( Util::colorFromLuaTable( state, lua_gettop( state ) ) );
        }

        lua_getfield( state, index, "origin" );
        if( lua_isinteger( state, - 1 ) ) {
            text.setOrigin( (OriginPosition)lua_tointeger( state, - 1 ) );
        }

        lua_getfield( state, index, "string" );
        if( !lua_isnil( state, - 1 ) ) {
            text.setString( lua_tostring( state, - 1 ) );
        }

        lua_pop( state, 7 );
    }
    else {
        cout << "Warning: Text wrapper can't be made out of non-table value" << endl;
    }

    return text;
}
