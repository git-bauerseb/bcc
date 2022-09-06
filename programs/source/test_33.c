int open(char * pathname, int flags);
int read(int fd, char * buffer, int count);
int write(int fd, void * buffer, int count);
int close(int fd);

char * buf ;

int main() {

    int zin;
    int cnt;

    buf = "                                                                                           ";
    zin = open("/home/sebastian/Programmierung/c/c_compiler/programs/source/test_33.prog", 0);

    if ( zin == - 1 ) {
        return 1;
    }

    while ( ( cnt = read ( zin, buf, 60 ) ) > 0 ) {
        write( 1, buf, cnt );
    }

    close(zin);

    return 0;
}