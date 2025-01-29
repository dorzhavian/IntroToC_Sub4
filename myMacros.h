#pragma once

//DONE
#define CHECK_RETURN_0(ptr) \
    if((ptr) == NULL) { \
        return 0; \
    }
//DONE
#define CHECK_MSG_RETURN_0(ptr, msg) \
    if ((ptr) == NULL) { \
        printf("%s", msg); \
        return 0; \
    }
//DONE
#define	FREE_CLOSE_FILE_RETURN_0(ptr, fp) { \
    free(ptr); \
    fclose(fp); \
    return 0; \
    }
//DONE
#define CLOSE_RETURN_0(fp) { \
    fclose(fp); \
    return 0;   \
}