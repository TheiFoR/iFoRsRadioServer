#ifndef LOGDEF_H
#define LOGDEF_H

#define STR(x) #x
#define CAT_STR(x, y) STR(x) "][" STR(y)

#define LOG_DECLARE(categoryName, subcategoryName) Q_LOGGING_CATEGORY(category##categoryName##subcategoryName, CAT_STR(categoryName, subcategoryName))

#endif // LOGDEF_H
