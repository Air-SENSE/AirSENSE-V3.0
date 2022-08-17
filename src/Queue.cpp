#include "Queue.h"
#define _DB_LOG_

Queue* Queue::Instance = nullptr;

void displayStruct(DataCore s)
{
    char buffer[sizeof(DataCore)];
    char* ptr = &buffer[0];
    memcpy(ptr, &s, sizeof(DataCore));
    for(int i = 0; i< sizeof(DataCore); i++)
    {
        Serial.print(buffer[i],HEX);
    }
    Serial.println();
}

// Check if the queue is full
bool Queue::isFull() {
    if (front == 0 && rear == QUEUE_SIZE - 1) 
    {
        return true;
    }
    if (front == rear + 1) 
    {
        return true;
    }
    return false;
}
// Check if the queue is empty
bool Queue::isEmpty() {
    if (front == -1)
    return true;
    else
    return false;
}
// Adding an element
void Queue::enQueue(DataCore element) {
    if (isFull()) {
        // cout << "Queue is full" << endl;
    } 
    else {
        if (front == -1) front = 0;
        rear = (rear + 1) % QUEUE_SIZE;
        items[rear] = element;
        #ifdef _DB_LOG_
            displayStruct(element);
        #endif
    }
}
// Removing an element
DataCore Queue::deQueue() {
    DataCore element = {};
    if (isEmpty()) {
        // cout << "Queue is empty" << endl;
        return element;
    } 
    else {
        element = items[front];
        if (front == rear) {
            front = -1;
            rear = -1;
        }
        // Q has only one element,
        // so we reset the queue after deleting it.
        else {
            front = (front + 1) % QUEUE_SIZE;
        }
        return (element);
    }
}

void Queue::display() {
    // Function to display status of Circular Queue
    int i;
    if (isEmpty()) {
        // cout << endl
        //     << "Empty Queue" << endl;
    } 
    else {
        // cout << "Front -> " << front;
        // cout << endl
        //     << "Items -> ";
        for (i = front; i != rear; i = (i + 1) % QUEUE_SIZE)
            displayStruct(items[i]);
        displayStruct(items[i]);
        // cout << endl
        // << "Rear -> " << rear;
    }
}