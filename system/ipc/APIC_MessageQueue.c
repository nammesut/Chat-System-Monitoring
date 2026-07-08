////////////////////////////////////////////////////////////////////////////////////////////////////                             
///                                                 
/// \file       APIC_MessageQueue.c                               
/// \breif      Overview: Message queue functionalities                                     
///                                                 
//////////////////////////////////////////////////////////////////////////////////////////////////// 

#include <stdio.h>
#include <mqueue.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "APIC_MessageQueue.h"

#define MAX_SIZE 64

const char *queue_name = "/my_message_queue";


// H�?m tạo queue, trả v�? mqd_t
mqd_t APIC_MessageQueue::messageQueueCreate() {
    mqd_t mq;
    struct mq_attr attr = {0};
    attr.mq_flags = 0;          // blocking mode
    attr.mq_maxmsg = 5;         // s�? message tối đa
    attr.mq_msgsize = MAX_SIZE; // kích thước tối đa mỗi gói

    mq = mq_open(queue_name, O_CREAT | O_WRONLY, 0644, &attr);
    if (mq == (mqd_t)-1) {
        perror("mq_open (create)");
    }
    return mq; // caller phải mq_close
}

// H�?m gửi message (đã có queue m�? sẵn)
void APIC_MessageQueue::messageQueueSend(mqd_t mq, const char *msg) {
    if (mq_send(mq, msg, strlen(msg) + 1, 0) < 0) {
        perror("mq_send");
    }
    // mq_close(mq);
}

void APIC_MessageQueue::messageQueueReceive(char *buffer) {
    mqd_t mq;
    unsigned int priority;

    mq =  mq_open(queue_name, O_RDONLY);    // M�? queue
    if (mq == (mqd_t)-1) {
        perror("mq_open (receive)");
        return;
    }

    if (mq_receive(mq, buffer, MAX_SIZE, &priority) < 0) {
        perror("mq_receive");
        mq_close(mq);
        return;
    }

    // mq_close(mq);
    // mq_unlink(queue_name);   // Khoát khỏi mới dùng
}