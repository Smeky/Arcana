#include "CMessage.h"

CMessage::CMessage()
: m_msg         ( "EmptyMessage" )
, m_sender      ( nullptr )
{}

CMessage::CMessage( std::string msg, CMessageListener* sender )
: m_msg         ( msg )
, m_sender      ( sender )
{}

std::string CMessage::getMsg() const {
    return m_msg;
}

CMessageListener* CMessage::getSender() {
    return m_sender;
}
