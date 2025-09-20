#define SIZE 4096
#define AF_INNET 2
#define SOCKET_STREAM 1

int sendToFd(int output_fd, int input_fd, int size);
int openPath(char* filePath);


struct in_address {
    unsigned int s_addr;
};
struct sockaddress_in {
    unsigned short sin_family;
    unsigned short sin_port;
    struct in_address sin_addr;
    unsigned char sin_zero[8];
};



int sizeOfString(char* string){
    int i = 0;
    while(string[i] != '\0'){
        i++;
    }
    return i;
}
char* firstPointerToChar(char* string, char ch){
    int i = 0;
    while(string[i] != ch){
        i++;
    }
    return string+i;
}
int areStringEqual(char* a, char* b){
    int i = 0;
    while(a[i] != 0 && b[i] != 0){
        
        if(a[i] != b[i]) return 0;
        i++;
    }
    return 1;
}
int isHtmlFile(char* path) {
    char* dot = firstPointerToChar(path, '.');

    return dot && areStringEqual(dot, ".html");
}
int print(char* string){
    int size = sizeOfString(string);
    __asm__("mov %rdi, %rsi\n"
            "mov %rdx, %rdx\n"// We need to load the return value of sizeOfString, we just got lucky
            "mov $2, %rdi\n" //2 is the stdout fd
            "mov $1, %rax\n" //1 is the syscall number for write
            "syscall\n");
    return 0;
}
void writeToFd(int fd, char* text, int size){
    __asm__ (
    "mov $1, %rax\n"
    "syscall"
    );
}
int sendToFd(int output_fd, int input_fd, int size){
    __asm__ (
    "mov %rdi, %rdi\n" //Load the out fd
    "mov %rcx, %rsi\n" //Load the in fd
    "mov $40, %rax\n" //load the sendfile syscall
    "mov %rdx, %r10\n" //Load the size
    "xor %rdx, %rdx\n" //zero out the options
    "syscall"
    );
}

int serveFile(char* buffer, int client_fd){
    char* file = buffer + 5; 
    *firstPointerToChar(file, ' ') = 0;
    int opened_file_fd = openPath(file);
    if(isHtmlFile(file)){
       char* header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
        writeToFd(client_fd, header, sizeOfString(header)); 
    }
    sendToFd(client_fd, opened_file_fd, SIZE);
    return opened_file_fd;
}

void closeFd(int fd){
    __asm__ (
    "mov %rdi, %rdi\n"
    "mov $3, %rax\n"
    "syscall"
    );
}
int openPath(char* filePath){
    __asm__ (
    "mov %rdi, %rdi\n" //load file path (redundant but kept for sake of sanity)
    "mov $2, %rax\n"
    "xor %rdx, %rdx\n"
    "xor %rsi, %rsi\n"
    "syscall"
    );
}
void receive(int fd, char* message, int size){
    //This is a little bit cheaty
    //All the registers are already correct.
    __asm__ (
    "mov $0, %rax\n"
    "syscall"
    );
}
int acceptRequest(int sockFd, struct sockaddress_in*, int size){
    __asm__ (
    "mov $43, %rax\n"
    "syscall"
    );
}
int bindSocket(int fileFd, struct sockaddress_in*, int size){
    __asm__ (
    "mov $49, %rax\n"
    "syscall"
    );
}
int listenPort(int fileFd, int backlog){
    __asm__ (
    "mov $50, %rax\n"
    "syscall"
    );
}
int makeSocket(int domain, int type, int protocol){
    __asm__ (
    "mov $41, %rax\n"
    "syscall"
    );
}

int main(void){
    char buffer[SIZE];

    print("Hello, World!\n");

    int socket_fd = makeSocket(AF_INNET, SOCKET_STREAM, 0);
    if (socket_fd < 0) return socket_fd;
    
    struct sockaddress_in addr = {AF_INNET, 0x2823, 0};
    if(bindSocket(socket_fd, &addr, sizeof(addr)) < 0) {
        print("failed to bind to socket.");
        return -1;
    }
    listenPort(socket_fd, 2);
    while(1){
        int client_fd = acceptRequest(socket_fd, 0, 0);
        receive(client_fd, buffer, SIZE);
        int ret_fd = serveFile(buffer, client_fd);

        print("Sent a file!\n");
        
        closeFd(ret_fd);
        closeFd(client_fd);
        
    }
    closeFd(socket_fd);
}