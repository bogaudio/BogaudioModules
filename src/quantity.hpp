#include "rack0.hpp"
#include "app.hpp"
#include "math.hpp"
#include "event.hpp"
#include <stdarg.h>
#include "rack0.hpp"
#include "math.hpp"
#include "event.hpp"

std::string stringf(const char *format, ...);

/** A Widget representing a float value */
struct QuantityWidget: rack::widget::Widget {
    float value = 0.0;
    float minValue = 0.0;
    float maxValue = 1.0;
    float defaultValue = 0.0;
    std::string label;
    /** Include a space character if you want a space after the number, e.g. " Hz" */
    std::string unit;
    /** The decimal place to round for displaying values.
    A precision of 2 will display as "1.00" for example.
    */
    int precision = 2;

    QuantityWidget();
    void setValue(float value);
    void setLimits(float minValue, float maxValue);
    void setDefaultValue(float defaultValue);
    /** Generates the display value */
    std::string getText();
};


