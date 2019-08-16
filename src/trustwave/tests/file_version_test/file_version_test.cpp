
extern "C" {
int get_ver(const char* path, char* res);
}

int main(int,char**)
{
    char *res=new char[1024];
    get_ver("/tmp/file", res);
    return 0;
}
