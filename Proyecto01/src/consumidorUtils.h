#ifndef CONSUMER_FLAG_UTILS
#define CONSUMER_FLAG_UTILS


int getBufferReadIndex(struct Flags* flags)
{
  return flags->address[BUFFER_READ_INDEX];
}

int checkBufferIndexForRead(struct Flags* flags, const int bufferSize)
{
  if(flags->address[BUFFER_READ_INDEX] == flags->address[BUFFER_WRITE_INDEX])
  {
    warn("The index to write=[%d] will reach the index to read=[%d] on the buffer.", flags->address[BUFFER_READ_INDEX], flags->address[BUFFER_WRITE_INDEX]);
    return 0;
  }
  else
    return 1;
}

void increaseBufferReadIndex(struct Flags* flags, const int bufferSize)
{
  flags->address[BUFFER_READ_INDEX] = (flags->address[BUFFER_READ_INDEX] + 1) % bufferSize;
}

int increaseConsumidor(struct Flags* flags)
{
  return increaseSubject(flags, CONSUMERS_INDEX, CONSUMERS_COUNT);
}

void decreaseConsumidor(struct Flags* flags)
{
  decreaseSubject(flags, PRODUCERS_COUNT);
}

int readMessage(struct Buffer* buffer, struct Flags* flags, char** message)
{
  int readIndex = -1;
  waitSemaphore(buffer->semId);
  if(1 == checkBufferIndexForRead(flags, BUFFER_SIZE))
  {
    *message = (char*) calloc(BUFFER_ROW_SIZE, sizeof(char));
    readIndex = getBufferReadIndex(flags);
    char* address = buffer->address + readIndex * BUFFER_ROW_SIZE;
    memcpy(*message, address, BUFFER_ROW_SIZE);
    increaseBufferReadIndex(flags, BUFFER_SIZE);
  }
  signalSemaphore(buffer->semId);
  return readIndex;
}

#endif
