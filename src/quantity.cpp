#include "quantity.hpp"
#include "event.hpp"

std::string stringf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    // Compute size of required buffer
    int size = vsnprintf(NULL, 0, format, args);
    va_end(args);
    if (size < 0)
        return "";
    // Create buffer
    std::string s;
    s.resize(size);
    va_start(args, format);
    vsnprintf(&s[0], size + 1, format, args);
    va_end(args);
    return s;
}

QuantityWidget::QuantityWidget() {
    rack::event::Change e;
    onChange(e);
}

void QuantityWidget::setValue(float value) {
    this->value = rack::math::clamp(value, fminf(minValue, maxValue), fmaxf(minValue, maxValue));
    rack::event::Change e;
    onChange(e);
}

void QuantityWidget::setLimits(float minValue, float maxValue) {
    this->minValue = minValue;
    this->maxValue = maxValue;
}

void QuantityWidget::setDefaultValue(float defaultValue) {
    this->defaultValue = defaultValue;
    setValue(defaultValue);
}

std::string QuantityWidget::getText() {
    std::string text = label;
    text += ": ";
    text += stringf("%.*f", precision, value);
    text += unit;
    return text;
}
