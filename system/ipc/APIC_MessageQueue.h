////////////////////////////////////////////////////////////////////////////////////////////////////
///
///	\file		APIC_MessageQueue.h
///	\brief		Overview: Interface of the APIC_MessageQueue class
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __APIC_MESSAGE_QUEUE_H__
#define __APIC_MESSAGE_QUEUE_H__

#include <mqueue.h> 

class APIC_MessageQueue {
public:
    /// \brief Create a message queue.
    /// \return The message queue descriptor.
    mqd_t messageQueueCreate();

    /// \brief Send a message to the message queue.
    /// \param mq The message queue descriptor.
    /// \param msg The message to send.
    void messageQueueSend(mqd_t mq, const char *msg);

    /// \brief Receive a message from the message queue.
    /// \param buffer The buffer to store the received message.
    void messageQueueReceive(char *buffer);
};

#endif