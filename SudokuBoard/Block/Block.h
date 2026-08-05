#ifndef ADV_AD_A_BRANCH_BOUND_BLOCK_H
#define ADV_AD_A_BRANCH_BOUND_BLOCK_H
#pragma once

class Block {
public:
    Block();

    void setBlockValue(int value);

    [[nodiscard]] int getBlockValue() const;

    [[nodiscard]] bool getIsFilled() const;

    void setIsFilled(bool value);

    void resetBlock();

    void print() const;

private:
    int blockValue;
    bool isFilled;
};


#endif //ADV_AD_A_BRANCH_BOUND_BLOCK_H
