#ifndef PIPE_OP_H_
#define PIPE_OP_H_


int ReadPipe(int fd, void* buf, size_t len);

int WritePipe(int fd, void* buf, size_t len);

#endif /* end of include guard: PIPE_OP_H_ */
