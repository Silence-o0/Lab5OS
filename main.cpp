#include <iostream>
#include <cmath>
#include <unistd.h>
#include <fcntl.h>

using namespace std;

int g(int x) {
    if (x > 45)
        return 0;
    else if (x < 30)
        return x;
    else
        while(1){}
}


int f(int x) {
    if (x > 40)
        return 0;
    else if (x < 30)
        return x;
    else
        while(1){}
}


void f_proc(int input_fd, int output_fd) {
    dup2(input_fd, 0);
    dup2(output_fd, 1);

    int x;
    cin >> x;
    cout << f(x);
}

void g_proc(int input_fd, int output_fd) {
    dup2(input_fd, 0);
    dup2(output_fd, 1);

    int x;
    cin >> x;
    cout << g(x);
}

bool ask() {
    int v = 0;
    while(1) {
        cout << "1. Continue, 2. Finish, 3. Continue, not asking" << endl;
        cin >> v;
        switch (v) {
            case 1:
                return true;
            case 2:
                cout << "Finished with error";
                exit(-1);
            case 3:
                return false;
            default:
                cout << "Incorrect value";
        }
    }
}

void final_res(int result) {
    cout << "Result: " << result;
    exit(0);
}

int main() {
    int f_input_descriptors[2], f_output_descriptors[2], g_input_descriptors[2], g_output_descriptors[2];
    pipe(f_input_descriptors);
    pipe(f_output_descriptors);
    pipe(g_input_descriptors);
    pipe(g_output_descriptors);

    if (fork() != 0) {
        close(f_input_descriptors[0]);
        close(f_output_descriptors[1]);

        if (fork() != 0) {
            close(g_input_descriptors[0]);
            close(g_output_descriptors[1]);

            int x;
            cin >> x;
            string s = to_string(x);

            write(f_input_descriptors[1], s.c_str(), 11);
            write(g_input_descriptors[1], s.c_str(), 11);

            fcntl(f_output_descriptors[0], F_SETFL, O_NONBLOCK);
            fcntl(g_output_descriptors[0], F_SETFL, O_NONBLOCK);
            int f_read, g_read;
            char f_res[11], g_res[11];
            int saved_res[2] = {-1, -1};   // non-saved
            bool should_ask = true;

            while (1) {
                sleep(10);
                f_read = read(f_output_descriptors[0], f_res, 11);
                g_read = read(g_output_descriptors[0], g_res, 11);
                if (f_read > 0 and saved_res[0] != 0) {
                    close(f_output_descriptors[0]);
                    if (atoi(f_res) != 0) {
                        final_res(1);
                    }
                    else {
                        if (saved_res[1] == 0) {
                            final_res(0);
                        }
                        saved_res[0] = 0;
                    }
                }
                if (g_read > 0 and saved_res[1] != 0) {
                    close(g_output_descriptors[0]);
                    if (atoi(g_res) != 0) {
                        final_res(1);
                    }
                    else {
                        if (saved_res[0] == 0) {
                            final_res(0);
                        }
                        saved_res[1] = 0;
                    }
                }
                if (should_ask) {
                    should_ask = ask();
                }
            }
        }
        else {
            close(g_input_descriptors[1]);
            close(g_output_descriptors[0]);

            g_proc(g_input_descriptors[0], g_output_descriptors[1]);
        }
    }
    else {
        close(f_input_descriptors[1]);
        close(f_output_descriptors[0]);

        f_proc(f_input_descriptors[0], f_output_descriptors[1]);
    }
}
