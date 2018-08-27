#ifndef CMESSAGE_H
#define CMESSAGE_H

#include "string"

class CMessageListener;

class CMessage {
public:
                        CMessage    ();
                        CMessage    ( std::string msg, CMessageListener* sender = nullptr );
    virtual            ~CMessage    () {}

    std::string         getMsg      () const;
    CMessageListener*   getSender   ();

protected:
    std::string         m_msg;
    CMessageListener*   m_sender;

};

template <class T>
class CTypedMessage : public CMessage {
public:
        CTypedMessage   ( std::string msg, T value, CMessageListener* sender = nullptr )
        : CMessage  ( msg, sender )
        , m_value   ( value )
        {}

    T   getValue        () {
            return m_value;
        }

private:
    T   m_value;
};

#endif // CMESSAGE_H
