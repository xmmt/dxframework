#pragma once

class InputDevice {
public:
    void addPressedKey();
    void isKeyDown();
    void onKeyDown();
    void onMouseMove();
    void removePressed();

private:
    int game_;
    int keys_;
    int mouseMove_;
    int mouseOffset_;
    int mousePosition_;
    int mouseWheelD_;
};