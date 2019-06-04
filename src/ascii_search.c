/*
 * Author: Ming Li (adagio.ming@gmail.com)
 * Date: 2018/12/11
 * Note-
 *  1: code follows C99 standard.
 *  2: code has only been used on MacOS.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TRUE 0x1
#define FALSE 0x0

#define HEX 0x30
#define DECIMAL 0x40

typedef enum {
    OP_SUCCESS = 0x0,
    OPERATION_FAIL,
    MALLOC_ERR,
    NON_GRAPHIC_CHARACTER_ERR,
    NON_GRAPHIC_INDEX_ERR,
    INVAL_DATA_FORMAT_ERR,
    CHARACTER_NOT_FOUND_ERR,
    NULL_POINTER_ERR,
    INDEX_OVERFLOW_ERR,
} rc_code;

static const char *ascii[128] = {
    " NUL(null) [空字符]", " SOH(start of headline) [标题开始]", " STX(start of text) [正文开始]",
    " ETX(end of text) [正文结束]", " EOT(end of transmission) [传输结束]", " ENQ(enquiry) [请求]",
    " ACK(acknowledge) [响应]", " BEL(bell) [响铃]", " BS(backspace) [退格]", " HT(horizontal tab) [水平制表符]",
    " LF(NL line feed, new line) [换行符]", " VT(vertlcal tab) [垂直制表符]",
    " FF(NP form feed, new page) [换页符]", " CR(carriage return) [回车键]", " SO(shift out) [不用切换]",
    " SI(shift in) [启用切换]", " DLE(data link escape) [数据链路转义]", " DC1(device control 1) [设备控制1]",
    " DC2(device control 2) [设备控制2]", " DC3(device control 3) [设备控制3]", " DC4(device control 4) [设备控制4]",
    " NAK(negative acknowledge) [拒绝接收]", " SYN(synchronous idle) [同步空闲]",
    " ETB(end of trans. block) [结束传输块]", " CAN(cancel) [取消]", " EM(end of medium) [媒介结束]",
    " SUB(substitute) [替换]", " ESC(escape) [退出]", " FS(file separator) [文件分隔符]",
    " GS(group separator) [分组符]", " RS(record separator) [记录分割符]", " US(unit separator) [单元分割符]",
    " (space) [空格]", "! [叹号]", "\" [双引号]", "# [井号]", "$ [美元符]", "\% [百分号]",
    "& [and]", "' [闭单引号]", "( [左括号]", ") [右括号]", "* [星号]", "+ [加]", ", [逗号]",
    "- [减]", ". [句点]", "/ [斜杠]", "0 ", "1 ", "2 ", "3 ", "4 ", "5 ", "6 ", "7 ", "8 ", "9 ",
    ": [冒号]", "; [分号]", "< [小于]", "= [等于]", "> [大于]", "? [问号]", "@ [at]", "A ", "B ",
    "C ", "D ", "E ", "F ", "G ", "H ", "I ", "J ", "K ", "L ", "M ", "N ", "O ", "P ", "Q ",
    "R ", "S ", "T ", "U ", "V ", "W ", "X ", "Y ", "Z ", "[ [左中括号]", "\\ [反斜杠]", "] [右中括号]",
    "^ [脱字符]", "_ [下划线]", "` [开单引号]", "a [小写]", "b [小写]", "c [小写]", "d [小写]",
    "e [小写]", "f [小写]", "g [小写]", "h [小写]", "i [小写]", "j [小写]", "k [小写]", "l [小写]",
    "m [小写]", "n [小写]", "o [小写]", "p [小写]", "q [小写]", "r [小写]", "s [小写]", "t [小写]",
    "u [小写]", "v [小写]", "w [小写]", "x [小写]", "y [小写]", "z [小写]", "{ [左大括号]", "| [or]",
    "} [右大括号]", "~ [波浪符]", " DEL(delete) [回删]",
};


static void usage(void)
{
    printf("ascii usage:\n");
    printf("\tascii [character], return ascii index of given character,\n");
    printf("\tascii [string], return ascii index list of given string,\n");
    printf("\tascii -i [index], return ascii character of given index,\n");
    printf("\tascii -i [index1 index2 index3...], return a string with corresponding ascii values.\n");
}

static int array_data_conv(char **, int, int **);
static int str_data_conv(char *, int, int **);
static int is_hex_or_decimal(char *, int *);
static void print_err_msg(int err, void *msg);

/*
 * err: error code,
 * msg: error message
 */
static void print_err_msg(int err, void *msg) {
    msg = msg;

    switch (err) {
        case OPERATION_FAIL:
            printf("Error: Operation Failed!\n");
            break;
        case MALLOC_ERR:
            printf("Error: Memory Error!\n");
            break;
        case NON_GRAPHIC_CHARACTER_ERR:
            printf("Error: invalid index: [%s], only graphic number is allowed, specify it between 32-126\n", (char *)msg);
            break;
        case NON_GRAPHIC_INDEX_ERR:
            printf("Error: invalid index: [%d, 0x%x], only graphic number is allowed, specify it between 32-126\n", *(int *)msg, *(int *)msg);
            break;
        case INVAL_DATA_FORMAT_ERR:
            printf("Error: invalid data format[%s], Hex or Decimal only!\n", (char *)msg);
            break;
        case CHARACTER_NOT_FOUND_ERR:
            printf("Error: character %s not found in ASCII set\n", (char *)msg);
            break;
        case NULL_POINTER_ERR:
            printf("Error: NULL pointer given!\n");
            break;
        case INDEX_OVERFLOW_ERR:
            printf("index overflow[%d, 0x%x], specify it between 0-127\n", *(int *)msg, *(int *)msg);
            break;
        default:
            printf("Unknown Error message!\n");
            break;
    }
}

/*
 * in: single data string,
 * index_p: if wrong format given, points to the index.
 */
static int is_hex_or_decimal(char *in, int *format) {
    int pos;
    int len = strlen(in);
    int a, b, c;
    int data_format = format? *format : HEX; /* default data format */

    format = format;

    for (pos=0; pos<len; pos++) {
        a = in[pos] - '0';
        b = in[pos] - 'A';
        c = in[pos] - 'a';
        if (a < 0 || (a > 9 && b < 0) || (b > 5 && c < 0) || c > 5) {
            return FALSE;
        }

        if (data_format == HEX) continue;

        if ((b >= 0 && b <=5) || (c >= 0 && c <= 5)) {
            data_format = HEX;
        }
    }

    if(format) {
        *format = data_format;
    }

    return TRUE;
}

/*
 * in: argument strings in argv vector,
 * len: argument string count,
 * out: ascii index list.
 * function: convert argv vector string data into real data when argc == 3;
 */
static int array_data_conv(char **in, int len, int **out) {

    if (!out) {
        print_err_msg(NULL_POINTER_ERR, 0);
        return -1;
    }

    int *conv_data_p = (int *)malloc(len * sizeof(int));
    if (!conv_data_p) {
        print_err_msg(MALLOC_ERR, 0);
        return -1;
    }
    memset(conv_data_p, ~0x0, sizeof (*conv_data_p));

    int pos = 0;
    int data_format = DECIMAL;
    int step_base;
    char *current;
    int tail_idx = 0;

    /* trim each string argument */
    for (; pos<len; pos++) {
        current = in[pos];
        tail_idx = strlen(current) - 1;

        if (current[0] == '0' && (current[1] == 'x' || current[1] == 'X')) {
            if(data_format == DECIMAL) {
                data_format = HEX;
            }
            in[pos] += 2;
            continue;
        }

        if (current[tail_idx] == ',' || current[tail_idx] == ';') {
            tail_idx -= 1;
        }

        if (current[tail_idx] == 'h' || current[tail_idx] == 'H') {
            if(data_format == DECIMAL) {
                data_format = HEX;
            }
            in[pos][tail_idx] = 0;
        }
    }

    /* check data content */
    for(pos =0; pos<len; pos++) {
        if(!is_hex_or_decimal(in[pos], data_format == DECIMAL? &data_format : NULL)) {
            free(conv_data_p);
            conv_data_p = NULL;
            print_err_msg(INVAL_DATA_FORMAT_ERR, (void *)in[pos]);
            return -1;
        }
    }

    if(data_format == DECIMAL) {
        step_base = 10;
    } else if (data_format == HEX) {
        step_base = 16;
    } else {
        print_err_msg(0xff, 0);
        return -1;
    }

    for(pos=0; pos<len; pos++) {
        conv_data_p[pos] = strtol(in[pos], NULL, step_base);
    }

    *out = conv_data_p;

    return OP_SUCCESS;
}

/*
 * in: argv[2]
 * out: index list array.
 * function: convert string argv vector into real data when argc > 3.
 */
static int str_data_conv(char *in, int count, int **out) {
    if (!out) {
        print_err_msg(NULL_POINTER_ERR, 0);
        return -1;
    }

    int *conv_data_p;
    char *current;
    int data_format = DECIMAL;
    char *delimiter = ",;";
    int pos;
    int change;
    int step_base;
    int word_hex_header_lock;
    char *c;


    conv_data_p = (int *) malloc(count * sizeof (int));
    memset(conv_data_p,  ~0x0, sizeof *conv_data_p);

    current = strtok(in, delimiter);
    c = current;
    for (; *c; c++) {
        if((data_format == DECIMAL) && ((*c == '0') && (*(c+1) == 'X' || *(c+1) == 'x'))) {
            data_format = HEX;
            current += 2;
        }
        if((data_format == DECIMAL) && ((*c == 'h' || *c == 'H') && *(c+1) == 0)) {
            data_format = HEX;
            *c = 0;
        }
    }
    if(!is_hex_or_decimal(current, data_format == DECIMAL? &data_format : NULL)) {
        free(conv_data_p);
        conv_data_p = NULL;
        print_err_msg(INVAL_DATA_FORMAT_ERR, (void *)current);
        return -1;
    }

    if (data_format == DECIMAL) {
        change = 1;
        step_base = 10;
    } else {
        change = 0;
        step_base = 16;
    }

    conv_data_p[0] = strtol(current, NULL, step_base);

    pos = 1;

    while((current = strtok(NULL, delimiter)) != NULL) {
        c = current;
        word_hex_header_lock = FALSE;
        for (; *c; c++) {
            if((*c == '0') && (*(c+1) == 'X' || *(c+1) == 'x')) {
                data_format = HEX;
                current += 2;
                word_hex_header_lock = TRUE;
                continue;
            }
            if(!word_hex_header_lock  && ((*c == 'h' || *c == 'H') && *(c+1) == 0)) {
                data_format = HEX;
                *c = 0;
            }
        }

        if(!is_hex_or_decimal(current, data_format == DECIMAL? &data_format : NULL)) {
            free(conv_data_p);
            conv_data_p = NULL;
            print_err_msg(INVAL_DATA_FORMAT_ERR, (void *)current);
            return -1;
        }
        if(data_format == HEX && step_base == 10) {
            change = pos;
            step_base = 16;
            int a;
            int b;
            int pow;
            int i;
            int sum;
            for(; change>=0; change--) {
                a = conv_data_p[change] / 10;
                b = conv_data_p[change] % 10;
                sum = b;
                pow = 0;
                while(a>0) {
                    pow += 1;
                    b = a % 10;
                    a /= 10;
                    for(i=0; i<pow; i++) {
                        b <<= 4;
                    }
                    sum += b;
                }

                conv_data_p[change] = sum;
            }
            conv_data_p[pos++] = strtol(current, NULL, step_base);
        } else {
            conv_data_p[pos++] = strtol(current, NULL, step_base);
        }
    }

    *out = conv_data_p;

    return OP_SUCCESS;
}

int main(int argc, char **argv)
{
    if (argc == 1){
        /* This is for special case */
        printf("(space) ascii index:\n");
        printf("Dec: 32\n");
        printf("Oct: 40\n");
        printf("Hex: 20\n");
        return 1;
    } else if (argc == 2) {

        if(!strncmp(argv[1], "-?", 2) || !strncmp(argv[1], "--help", 6) || !strncmp(argv[1], "-h", 2)) {
            usage();
            return -1;
        }

        int multi_args = FALSE;
        int arg_count = strlen(argv[1]);
        int found = FALSE;

        if(arg_count > 1) multi_args = TRUE;

        int i;
        char ch = 0;

        if (multi_args) {
            int *id_list = (int *)malloc(arg_count * sizeof(int));

            if (id_list == NULL) {
                print_err_msg(MALLOC_ERR, 0);
                return -1;
            }
            memset(id_list, 0x0, arg_count * sizeof (int));

            for (int pos=0; pos<arg_count; pos++) {
                ch = argv[1][pos];

                if(ch < 32 || ch == 127) {
                    print_err_msg(NON_GRAPHIC_CHARACTER_ERR, (void *)&ch);
                    return -1;
                }

                for (i=0; i<128; i++) {
                    if(ascii[i][0] == ch) {
                        found = TRUE;
                        break;
                    }
                }
                if(found) {
                    id_list[pos] = i;
                    found = FALSE;
                } else {
                    print_err_msg(CHARACTER_NOT_FOUND_ERR, (void *)&ch);
                    return -1;
                }
            }

            printf("Result:\nDecimal: ");
            for(i=0; i<arg_count; i++) {
                printf("%d, ", id_list[i]);
            }
            printf("\nHex: ");
            for(i=0; i<arg_count; i++) {
                printf("0x%x, ", id_list[i]);
            }
            printf("\nOct: ");
            for(i=0; i<arg_count; i++) {
                printf("%o, ", id_list[i]);
            }
            putchar('\n');
            free(id_list);
            id_list = NULL;

            return 0;
        } else {
            found = FALSE;
            for(i=0; i<128; i++) {
                if(ascii[i][0] == argv[1][0]) {
                    found = TRUE;
                    break;
                }
            }
            if (found) {
                printf("Index: %d\n", i);
                printf("Hex: 0x%x\n", i);
                printf("Oct: %o\n", i);
            } else {
                print_err_msg(CHARACTER_NOT_FOUND_ERR, (void *)&ch);
                return -1;
            }
        }
    } else if(argc == 3 && !strcmp(argv[1], "-i")) {
        char *param = argv[2];
        int len = strlen(argv[2]);

        int multi_args = FALSE;
        int arg_count = 0;
        int *index_list = NULL;

        int i;

        for (i=0; i<len-1; i++) {
            if (argv[2][i] == ',' || argv[2][i] == ';') {
                if (multi_args == FALSE) {
                    multi_args = TRUE;
                }
                arg_count++;
            }
        }
        arg_count += 1;
        if (multi_args) {
            if((str_data_conv(argv[2], arg_count, &index_list)) != OP_SUCCESS) {
                return -1;
            }

            for(i=0; i<arg_count; i++) {
                if(index_list[i] > 127) {
                    print_err_msg(INDEX_OVERFLOW_ERR, (void *)&index_list[i]);
                    return -1;
                } else if(index_list[i]<32 || index_list[i]==127) {
                    print_err_msg(NON_GRAPHIC_INDEX_ERR, (void *)&index_list[i]);
                    return -1;
                }
            }
            printf("Result String:\n");
            for (i=0; i<arg_count; i++) {
                putchar(ascii[index_list[i]][0]);
            }
            printf("\nHex Dump:\n");
            for (i=0; i<arg_count; i++) {
                printf("0x%x ", index_list[i]);
            }
            printf("\nOct Dump:\n");
            for (i=0; i<arg_count; i++) {
                printf("%o ", index_list[i]);
            }
            printf("\n");
            free(index_list);
            index_list = NULL;
            return 0;
        } else {
            if((array_data_conv(&argv[2], 1, &index_list)) != OP_SUCCESS) {
                return -1;
            }
            if (*index_list >=0 && *index_list < 128) {
                printf("%d indexs to %s\n", *index_list, ascii[*index_list]);
                printf("Oct: %o\n", *index_list);
                printf("Hex: %x\n", *index_list);
            } else {
                print_err_msg(INDEX_OVERFLOW_ERR, (void *)index_list);
            }
        }
    } else if (argc > 3 && !strcmp(argv[1], "-i")){
        int i;
        int *index_list;
        if((array_data_conv(&argv[2], argc-2, &index_list)) != OP_SUCCESS) {
            return -1;
        }

        for(i=0; i<argc-2; i++) {
            if(index_list[i] > 127) {
                print_err_msg(INDEX_OVERFLOW_ERR, (void *)&index_list[i]);
                return -1;
            } else if(index_list[i]<32 || index_list[i]==127) {
                print_err_msg(NON_GRAPHIC_INDEX_ERR, (void *)&index_list[i]);
                return -1;
            }
        }
        printf("Result String:\n");
        for(i=0; i<argc-2; i++) {
            putchar(ascii[index_list[i]][0]);
        }
        printf("\nHex Dump:\n");
        for(i=0; i<argc-2; i++) {
            printf("0x%x ", index_list[i]);
        }
        printf("\nOct Dump:\n");
        for(i=0; i<argc-2; i++) {
            printf("%o ", index_list[i]);
        }
        printf("\n");
        free(index_list);
        index_list = NULL;
    } else {
        usage();
    }

    return 0;
}

