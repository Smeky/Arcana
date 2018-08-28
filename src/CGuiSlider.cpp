#include "CGuiSlider.h"

#include <lua.hpp>

#include <CGame.h>
#include <Utility.h>
#include <LuaWrapper.h>

#include <iostream>
using namespace std;

CGuiSlider::CGuiSlider()
: m_spriteSliderID  ( 0 )
, m_orientation     ( SliderOrint::HORIZONTAL )
, m_value           ( 0.0 )
, m_locked          ( false )
, m_mouseover       ( false )
, m_pressed         ( false )
, m_clicked         ( false )
, m_checked         ( false )
, m_wasPressed      ( false )
{
    m_type = GuiType::GUI_SLIDER;

    for( size_t& ID : m_luaFuncIDs ) {
        ID = 0;
    }
}

void CGuiSlider::handleInput( sf::Event event ) {
    if( !m_active || m_locked ) return;

    for( auto it : m_childs ) {
        it->handleInput( event );
    }

    if( m_mouseover ) {
        if( event.type == sf::Event::MouseButtonPressed ) {
            if( event.mouseButton.button == sf::Mouse::Left ) {
                m_pressed = true;
            }
        }
    }

    if( event.type == sf::Event::MouseMoved ) {
        if( m_pressed ) {
            sf::Vector2i mousePos( event.mouseMove.x, event.mouseMove.y );

            moveSlider( mousePos );
        }
        else {
            bool wasMouseover = m_mouseover;

            if( m_sliderBox.contains( event.mouseMove.x, event.mouseMove.y ) ) {
                m_mouseover = true;

                if( !wasMouseover ) {
                    playSound( CGuiSlider::ONMOUSEOVER );
                }
            }
            else {
                m_mouseover = false;
            }
        }
    }
    else if( event.type == sf::Event::MouseButtonReleased ) {
        if( event.mouseButton.button == sf::Mouse::Left ) {
            if( m_pressed && m_mouseover ) {
                m_clicked = true;
            }

            m_pressed = false;
        }
    }
}

void CGuiSlider::update( const sf::Time& delta ) {
    if( !m_active ) return;

    for( auto it : m_childs ) {
        it->update( delta );
    }

    if( m_clicked ) {
        playSound( CGuiSlider::ONCLICK );

        m_clicked = false;
    }

    if( m_spriteSliderID != 0 ) {
        CSpriteWrapper* sprite = getSprite( m_spriteSliderID );

        if( sprite != nullptr ) {
            // Update texture clip rectangle based on button state
            if( m_locked ) {
                sprite->setClip( sf::IntRect( m_buttonClipSize.x * 3, 0, m_buttonClipSize.x, m_buttonClipSize.y ) );
            }
            else if( m_pressed ) {
                sprite->setClip( sf::IntRect( m_buttonClipSize.x * 2, 0, m_buttonClipSize.x, m_buttonClipSize.y ) );
            }
            else if( m_mouseover ) {
                sprite->setClip( sf::IntRect( m_buttonClipSize.x * 1, 0, m_buttonClipSize.x, m_buttonClipSize.y ) );
            }
            else {
                sprite->setClip( sf::IntRect( m_buttonClipSize.x * 0, 0, m_buttonClipSize.x, m_buttonClipSize.y ) );
            }
        }
    }

    m_wasPressed    = m_pressed;
}

void CGuiSlider::draw( sf::RenderTarget& window, sf::RenderStates states ) const {
if( !m_shown ) return;

    if( m_parent == nullptr ) {
        states.transform    *= getTransform();
    }
    else {
        sf::Transformable transformable;
        transformable.setPosition( getRelativePos() );
        states.transform    *= transformable.getTransform();
    }

    for( auto& it : m_sprites ) {
        if( it.getID() != m_spriteSliderID ) {
            window.draw( it, states );
        }
    }

    // Draw the slider with correct transformation
    if( m_spriteSliderID != 0 ) {
        for( auto& it : m_sprites ) {
            if( it.getID() == m_spriteSliderID ) {
                sf::Transformable transformable;
                transformable.setPosition( m_sliderBox.left, m_sliderBox.top );

                sf::RenderStates temp;
                temp.transform *= transformable.getTransform();

                window.draw( it, temp );

                break;
            }
        }

    }

    for( auto& it : m_texts ) {
        window.draw( it, states );
    }

    for( auto it : m_childs ) {
        window.draw( *it, states );
    }
}

void CGuiSlider::registerFunction( SliderFunction type, size_t ID ) {
    m_luaFuncIDs[ type ]    = ID;
}

size_t CGuiSlider::addSliderTexture( lua_State* state, int index, bool* result ) {
    bool success;

    CSpriteWrapper sprite = CSpriteWrapper::loadFromTable( state, index, &success );

    if( success ) {
        m_buttonClipSize    = sprite.getTexture()->getSize();
        m_buttonClipSize.x  = m_buttonClipSize.x / 4;

        sprite.setClip( sf::IntRect( 0, 0, m_buttonClipSize.x, m_buttonClipSize.y ) );

        sprite.setSize( m_sliderBox.width, m_sliderBox.height );
        sprite.setID( getNewID() );


        m_sprites.push_back( sprite );

        m_spriteSliderID = sprite.getID();

        return sprite.getID();
    }
    else {
        return 0;
    }
}

bool CGuiSlider::setSliderTexture( size_t ID ) {
    for( const auto& it : m_sprites ) {
        if( it.getID() == ID ) {
            m_spriteSliderID = ID;

            return true;
        }
    }

    return false;
}

void CGuiSlider::setSliderSize( const sf::Vector2f& size ) {
    m_sliderBox.width   = size.x;
    m_sliderBox.height  = size.y;

    if( m_orientation == SliderOrint::HORIZONTAL ) {
        m_sliderBox.top     = m_pos.y + m_size.y / 2 - m_sliderBox.height / 2;
    }
    else {
        m_sliderBox.left    = m_pos.x + m_size.x / 2 - m_sliderBox.width / 2;
    }

    updateSlider();
}

void CGuiSlider::setOrientation( SliderOrint orientation ) {
    m_orientation   = orientation;
}

void CGuiSlider::setRange( const sf::Vector2f& range ) {
    m_range         = range;
}

void CGuiSlider::setValue( float value ) {
    m_value         = value;

    updateSlider();
}

float CGuiSlider::getValue() const {
    return m_value;
}

void CGuiSlider::setSound( ButtonSoundType type, const std::string& ID ) {
    m_sounds[ type ]    = ID;
}

void CGuiSlider::setLocked( bool locked ) {
    m_locked    = locked;
}

bool CGuiSlider::isLocked() const {
    return m_locked;
}

CGuiSlider* CGuiSlider::setupFromTable( lua_State* state, int index, bool* result, const CGuiElement* parent ) {
    CGuiSlider* slider = new CGuiSlider();

    bool success = CGuiElement::loadFromTable( state, index, slider, parent );

    if( success ) {
        bool locked = false;
        lWrapper::valueFromLuaTable<bool>( state, index, "locked", locked );
        slider->setLocked( locked );

        slider->setSliderSize( Util::vectorFromTable<float>( state, index, "sliderSize" ) );

        /** Orientation */
        CGuiSlider::SliderOrint orientation = CGuiSlider::SliderOrint::HORIZONTAL;
        lWrapper::valueFromLuaTable<CGuiSlider::SliderOrint>( state, index, "orientation", orientation );
        slider->setOrientation( orientation );

        /** Range */
        lua_getfield( state, index, "sliderRange" );
        if( lua_istable( state, - 1 ) ) {
            slider->setRange( Util::vectorFromTable<float>( state, lua_gettop( state ) ) );
        }
        else {
            if( orientation == CGuiSlider::SliderOrint::HORIZONTAL ) {
                slider->setRange( sf::Vector2f( 0, slider->getSize().x ) );
            }
            else {
                slider->setRange( sf::Vector2f( 0, slider->getSize().y ) );
            }
        }

        lua_getfield( state, index, "sliderTex" );
        if( !lua_isnil( state, - 1 ) ) {
            slider->addSliderTexture( state, lua_gettop( state ) );
        }

        float value = 0.0;
        lWrapper::valueFromLuaTable<float>( state, index, "value", value );
        slider->setValue( value );

        lua_getfield( state, index, "sounds" );
        if( lua_istable( state, - 1 ) ) {
            for( lua_pushnil( state ); lua_next( state, - 2 ) != 0; lua_pop( state, 1 ) ) {
                // Copy key
                lua_pushvalue( state, - 2 );

                const std::string key = lua_tostring( state, - 1 );

                lua_pop( state, 1 );

                if( key == "onClick" ) {
                    slider->setSound( CGuiSlider::ONCLICK, lua_tostring( state, - 1 ) );
                }
                else if( key == "onMouseover" ) {
                    slider->setSound( CGuiSlider::ONMOUSEOVER, lua_tostring( state, - 1 ) );
                }
            }
        }

        lua_getfield( state, index, "functions" );
        if( lua_istable( state, - 1 ) ) {
            for( lua_pushnil( state ); lua_next( state, - 2 ) != 0; lua_pop( state, 1 ) ) {
                std::string key;

                lua_pushvalue( state, - 2 );
                if( lua_isstring( state, - 1 ) ) {
                    key = lua_tostring( state, - 1 );
                }

                lua_pop( state, 1 );

                lua_pushvalue( state, - 1 );

                if( !key.empty() ) {
                    if( key == "onMove" )   slider->registerFunction( SliderFunction::SFUNC_ONMOVE, CGame::InterfaceSystem.registerLuaFunction( state ) );
                }
            }
        }

        lua_pop( state, 4 );
    }

    if( result != nullptr ) {
        *result = success;
    }

    return slider;

}

/** LUA */


int CGuiSlider::luaAddSliderTexture( lua_State* state ) {
    int argc = lua_gettop( state );

    size_t result = 0;

    if( argc != 3 ) {
        std::cout << "Error: Unable to add button sprite to slider. Wrong amount of arguments" << std::endl;
    }
    else {
        if( lua_islightuserdata( state, argc - 1 ) ) {
            CGuiSlider* slider = (CGuiSlider*)lua_touserdata( state, argc - 1 );

            if( slider->getType() == GuiType::GUI_SLIDER ) {
                result = slider->addSliderTexture( state, argc );
            }
            else {
                std::cout   << "Error: Wrong gui element type. Expected "
                            << CGuiElement::guiTypeToString( GuiType::GUI_SLIDER )
                            << ", got: " << CGuiElement::guiTypeToString( slider->getType() )
                            << std::endl;
            }
        }
    }

    lua_pop( state, argc );

    lua_pushinteger( state, result );

    return 1;
}

int CGuiSlider::luaSetSliderTexture( lua_State* state ) {
    int argc = lua_gettop( state );

    bool result = false;

    if( argc == 3 ) {
        CGuiSlider* slider = (CGuiSlider*)lua_touserdata( state, argc - 1 );

        if( slider->getType() == GuiType::GUI_SLIDER ) {
            result = slider->setSliderTexture( lua_tointeger( state, argc ) );
        }
        else {
            std::cout   << "Error: Wrong gui element type. Expected "
                        << CGuiElement::guiTypeToString( GuiType::GUI_SLIDER )
                        << ", got: " << CGuiElement::guiTypeToString( slider->getType() )
                        << std::endl;
        }
    }

    lua_pop( state, argc );

    lua_pushboolean( state, result );

    return 1;
}

int CGuiSlider::luaSetOrientation( lua_State* state ) {
    int argc = lua_gettop( state );

    if( argc == 3 ) {
        CGuiSlider* slider = (CGuiSlider*)lua_touserdata( state, argc - 1 );

        if( slider->getType() == GuiType::GUI_SLIDER ) {
            slider->setOrientation( (SliderOrint)lua_tointeger( state, argc ) );
        }
        else {
            std::cout   << "Error: Wrong gui element type. Expected "
                        << CGuiElement::guiTypeToString( GuiType::GUI_SLIDER )
                        << ", got: " << CGuiElement::guiTypeToString( slider->getType() )
                        << std::endl;
        }
    }

    lua_pop( state, argc );

    return 0;
}

int CGuiSlider::luaSetRange( lua_State* state ) {
    int argc = lua_gettop( state );

    if( argc == 3 ) {
        CGuiSlider* slider = (CGuiSlider*)lua_touserdata( state, argc - 1 );

        if( slider->getType() == GuiType::GUI_SLIDER ) {
            slider->setRange( Util::vectorFromTable<float>( state, argc ) );
        }
        else {
            std::cout   << "Error: Wrong gui element type. Expected "
                        << CGuiElement::guiTypeToString( GuiType::GUI_SLIDER )
                        << ", got: " << CGuiElement::guiTypeToString( slider->getType() )
                        << std::endl;
        }
    }

    lua_pop( state, argc );

    return 0;
}

int CGuiSlider::luaSetValue( lua_State* state ) {
    int argc = lua_gettop( state );

    if( argc == 3 ) {
        CGuiSlider* slider = (CGuiSlider*)lua_touserdata( state, argc - 1 );

        if( slider->getType() == GuiType::GUI_SLIDER ) {
            slider->setValue( lua_tonumber( state, argc ) );
        }
        else {
            std::cout   << "Error: Wrong gui element type. Expected "
                        << CGuiElement::guiTypeToString( GuiType::GUI_SLIDER )
                        << ", got: " << CGuiElement::guiTypeToString( slider->getType() )
                        << std::endl;
        }
    }

    lua_pop( state, argc );

    return 0;
}

int CGuiSlider::luaGetValue( lua_State* state ) {
    int argc = lua_gettop( state );

    float value = 0.0;

    if( argc == 2 ) {
        CGuiSlider* slider = (CGuiSlider*)lua_touserdata( state, argc );

        if( slider->getType() == GuiType::GUI_SLIDER ) {
            value   = slider->getValue();
        }
        else {
            std::cout   << "Error: Wrong gui element type. Expected "
                        << CGuiElement::guiTypeToString( GuiType::GUI_SLIDER )
                        << ", got: " << CGuiElement::guiTypeToString( slider->getType() )
                        << std::endl;
        }
    }

    lua_pop( state, argc );

    lua_pushnumber( state, value );

    return 1;
}

int CGuiSlider::luaSetSound( lua_State* state ) {
    int argc = lua_gettop( state );

    if( argc == 4 ) {
        if( lua_islightuserdata( state, argc - 1 ) ) {
            CGuiSlider*     slider      = (CGuiSlider*)lua_touserdata( state, argc - 2 );
            std::string     ID          = lua_tostring( state, argc - 1 );
            ButtonSoundType type        = (ButtonSoundType)lua_tointeger( state, argc );

            if( slider != nullptr ) {
                slider->setSound( type, ID );
            }
        }
        else {
            std::cout << "Error: Unable to set sound to slider. Light user data expected" << std::endl;
        }
    }
    else {
        std::cout << "Error: Unable to set sound to slider. Wrong amount of arguments" << std::endl;
    }

    lua_pop( state, argc );

    return 0;
}

int CGuiSlider::luaSetLocked( lua_State* state ) {
    int argc = lua_gettop( state );

    if( argc == 3 ) {
        CGuiSlider* slider = (CGuiSlider*)lua_touserdata( state, argc - 1 );

        if( slider->getType() == GuiType::GUI_SLIDER ) {
            slider->setLocked( lua_toboolean( state, argc ) );
        }
        else {
            std::cout   << "Error: Wrong gui element type. Expected "
                        << CGuiElement::guiTypeToString( GuiType::GUI_SLIDER )
                        << ", got: " << CGuiElement::guiTypeToString( slider->getType() )
                        << std::endl;
        }
    }

    lua_pop( state, argc );

    return 0;
}

int CGuiSlider::luaIsLocked( lua_State* state ) {
    int argc = lua_gettop( state );

    bool result = false;

    if( argc == 2 ) {
        CGuiSlider* slider = (CGuiSlider*)lua_touserdata( state, argc );

        if( slider->getType() == GuiType::GUI_SLIDER ) {
            result = slider->isLocked();
        }
        else {
            std::cout   << "Error: Wrong gui element type. Expected "
                        << CGuiElement::guiTypeToString( GuiType::GUI_SLIDER )
                        << ", got: " << CGuiElement::guiTypeToString( slider->getType() )
                        << std::endl;
        }
    }

    lua_pop( state, argc );

    lua_pushboolean( state, result );

    return 1;
}

/** PRIVATE */

CSpriteWrapper* CGuiSlider::getSprite( size_t ID ) {
    if( ID != 0 ) {
        for( auto& it : m_sprites ) {
            if( it.getID() == ID ) {
                return &it;
            }
        }
    }

    return nullptr;
}

void CGuiSlider::moveSlider( sf::Vector2i mousePos ) {
    float prevValue = m_value;

    if( m_orientation == SliderOrint::HORIZONTAL ) {
        // Make sure slider doesn't outside its range
        if( mousePos.x < m_pos.x + m_range.x ) {
            mousePos.x = m_pos.x + m_range.x;
        }
        else if( mousePos.x > m_pos.x + m_range.y ) {
            mousePos.x = m_pos.x + m_range.y;
        }

        m_value = ( mousePos.x - m_pos.x - m_range.x ) / m_range.y;
    }
    else {
        // Make sure slider doesn't outside its range
        if( mousePos.y < m_pos.x + m_range.x ) {
            mousePos.y = m_pos.x + m_range.x;
        }
        else if( mousePos.y > m_pos.x + m_range.y ) {
            mousePos.y = m_pos.x + m_range.y;
        }

        m_value = ( mousePos.y - m_pos.y - m_range.x ) / m_range.y;
    }

    // Slider has moved
    if( prevValue != m_value ) {
        CGame::InterfaceSystem.callLuaFunction( m_luaFuncIDs[ SliderFunction::SFUNC_ONMOVE ], this );
    }

    updateSlider();
}

void CGuiSlider::updateSlider() {
    if( m_orientation == SliderOrint::HORIZONTAL ) {
        m_sliderBox.left    = m_pos.x - m_sliderBox.width / 2 + m_value * m_range.y;
    }
    else {
        m_sliderBox.top     = m_pos.y - m_sliderBox.height / 2 + m_value * m_range.y;
    }
}

void CGuiSlider::playSound( ButtonSoundType type ) {
    CGame::AudioSystem.playSound( m_sounds[ type ], CAudioSystem::INTERFACE );
}
