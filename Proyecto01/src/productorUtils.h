#ifndef PRODUCER_FLAG_UTILS
#define PRODUCER_FLAG_UTILS


int checkBufferIndexForWrite(struct Flags* flags, const int bufferSize)
{
  int newIndex = (flags->address[BUFFER_WRITE_INDEX] + 1) % bufferSize;
  if(newIndex == flags->address[BUFFER_READ_INDEX])
  {
    warn("The index to write=[%d] will reach the index to read=[%d] on the buffer.", newIndex, flags->address[BUFFER_READ_INDEX]);
    return 0;
  }
  else
    return 1;
}

int getBufferWriteIndex(struct Flags* flags)
{
  return flags->address[BUFFER_WRITE_INDEX];
}

void increaseBufferWriteIndex(struct Flags* flags, const int bufferSize)
{
  flags->address[BUFFER_WRITE_INDEX] = (flags->address[BUFFER_WRITE_INDEX] + 1) % bufferSize;
}

int increaseProductor(struct Flags* flags)
{
  return increaseSubject(flags, PRODUCERS_INDEX, PRODUCERS_COUNT);
}

void decreaseProductor(struct Flags* flags)
{
  decreaseSubject(flags, PRODUCERS_COUNT);
}

int writeMessage(struct Buffer* buffer, struct Flags* flags, char* message)
{
  int writeIndex = -1;
  waitSemaphore(buffer->semId);
  if(0 == checkBufferIndexForWrite(flags, BUFFER_SIZE))
  {
    warn("Buffer is full, can't create more message\n");
  }
  else
  {
    writeIndex = getBufferWriteIndex(flags);
    char* address = buffer->address + writeIndex * BUFFER_ROW_SIZE;
    memcpy(address, message, BUFFER_ROW_SIZE);
    increaseBufferWriteIndex(flags, BUFFER_SIZE);
  }
  signalSemaphore(buffer->semId);
  return writeIndex;
}

#endif
