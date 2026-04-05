#ifndef _MERCHANT_H_
#define _MERCHANT_H_

// merchant.h
// Defines MerchantItem — one entry on a merchant's shelf.
// The shopping loop (runShop) and the four merchant functions live in IAIB.cpp
// so they have direct access to startGame's stock vectors and the Player.
//
// HOW TO ADD A NEW ITEM TYPE
// ─────────────────────────
// 1. Pick a short effect-tag string (e.g. "food", "meds", "bullet").
// 2. Add a matching case to the applyEffect switch inside runShop() in IAIB.cpp.
// 3. Build a MerchantItem using that tag and add it to the right stock vector
//    in the startGame constructor.

#include <string>
using namespace std;

struct MerchantItem {
    string name;        // shown in the numbered menu
    string description; // one-line flavour text printed beneath the name
    int    cost;        // trade-goods price per purchase
    int    stock;       // units available; counts down permanently across visits
    string effectTag;   // tag read by runShop()'s applyEffect block
    int    effectAmt;   // how much/many the effect grants

    MerchantItem(string nm, string desc, int c, int stk,
                 string tag, int amt = 1)
        : name(nm), description(desc), cost(c),
          stock(stk), effectTag(tag), effectAmt(amt) {}
};

#endif // _MERCHANT_H_
