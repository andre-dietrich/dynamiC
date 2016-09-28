#define IF(COND, THEN, ELSE) COND ? THEN : ELSE

#define SWITCH3(COND,                                                       \
                CASE1, BODY1,                                               \
                CASE2, BODY2,                                               \
                CASE3, BODY3,                                               \
                DEFAULT)      IF((COND == CASE1), BODY1,                    \
                                 IF((COND == CASE2), BODY2,                 \
                                    IF((COND == CASE3), BODY3,              \
                                       DEFAULT)))
