//! PSYQ=4.0 CC1=2.7.2 UNROLL=true
#include <game.h>
#include "chocobo_private.h"

ChocoboPrizeTable(
    D_800B22D0, 22, //
    {18, 1, 5, 0},  // Turbo Ether
    {10, 0, 5, 0},  // Hero Drink
    {9, 0, 5, 1},   // Elixir
    {1, 0, 5, 1},   // Counter
    {5, 1, 5, 1},   // Enemy Away
    {6, 1, 5, 1},   // Sneak Attack
    {14, 1, 5, 0},  // Swift Bolt
    {15, 1, 5, 0},  // Fire Veil
    {11, 0, 2, 0},  // Bolt Plume
    {15, 1, 5, 0},  // Fire Veil
    {20, 0, 20, 0}, // Phoenix Down
    {16, 0, 5, 0},  // Ice Crystal
    {17, 1, 5, 1},  // Megalixir
    {18, 0, 5, 0},  // Turbo Ether
    {0, 1, 5, 1},   // Sprint Shoes
    {14, 0, 5, 0},  // Swift Bolt
    {4, 1, 5, 1},   // Cat's Bell
    {9, 0, 5, 1},   // Elixir
    {7, 1, 5, 1},   // Chocobracelet
    {16, 1, 5, 0},  // Ice Crystal
    {3, 1, 5, 1},   // Precious Watch
    {2, 1, 5, 1}    // Magic Counter
);

ChocoboPrizeTable(
    D_800B232C, 20, //
    {8, 0, 20, 0},  // Ether
    {10, 0, 10, 0}, // Hero Drink
    {14, 1, 10, 0}, // Swift Bolt
    {15, 1, 10, 0}, // Fire Veil
    {12, 0, 10, 0}, // Fire Fang
    {16, 1, 10, 0}, // Ice Crystal
    {1, 0, 10, 1},  // Counter
    {5, 1, 5, 1},   // Enemy Away
    {11, 0, 10, 0}, // Bolt Plume
    {12, 0, 10, 0}, // Fire Fang
    {13, 0, 10, 0}, // Antarctic Wind
    {9, 0, 10, 1},  // Elixir
    {23, 0, 5, 0},  // Hi-Potion
    {8, 0, 20, 0},  // Ether
    {0, 1, 7, 1},   // Sprint Shoes
    {9, 0, 5, 1},   // Elixir
    {20, 0, 20, 0}, // Phoenix Down
    {12, 0, 10, 0}, // Fire Fang
    {4, 1, 7, 1},   // Cat's Bell
    {6, 1, 7, 1}    // Sneak Attack
);

ChocoboPrizeTable(
    D_800B2380, 15, //
    {8, 0, 30, 0},  // Ether
    {10, 0, 10, 0}, // Hero Drink
    {20, 0, 20, 0}, // Phoenix Down
    {18, 0, 5, 1},  // Turbo Ether
    {8, 0, 30, 0},  // Ether
    {23, 0, 5, 0},  // Hi-Potion
    {21, 0, 10, 0}, // Hyper
    {22, 0, 10, 0}, // Tranquilizer
    {23, 0, 5, 0},  // Hi-Potion
    {11, 0, 10, 1}, // Bolt Plume
    {12, 0, 10, 0}, // Fire Fang
    {13, 0, 10, 0}, // Antarctic Wind
    {9, 0, 10, 1},  // Elixir
    {5, 1, 5, 1},   // Enemy Away
    {23, 0, 5, 0}   // Hi-Potion
);

ChocoboPrizeTable(
    D_800B23C0, 10, //
    {23, 0, 5, 1},  // Hi-Potion
    {8, 0, 20, 1},  // Ether
    {21, 0, 10, 0}, // Hyper
    {22, 0, 10, 0}, // Tranquilizer
    {20, 0, 20, 0}, // Phoenix Down
    {11, 0, 5, 1},  // Bolt Plume
    {12, 0, 5, 0},  // Fire Fang
    {13, 0, 5, 0},  // Antarctic Wind
    {19, 0, 5, 1},  // Potion
    {20, 0, 20, 1}  // Phoenix Down
);

// clang-format off
u8 D_800B23EC[][16] = {
    _SF(16, "Sprint Shoes"),
    _SF(16, "Counter"),
    _SF(16, "Magic Counter"),
    _SF(16, "Precious Watch"),
    _SF(16, "Cat's Bell"),
    _SF(16, "Enemy Away"),
    _SF(16, "Sneak Attack"),
    _SF(16, "Chocobracelet"),
    _SF(16, "Ether"),
    _SF(16, "Elixir"),
    _SF(16, "Hero Drink"),
    _SF(16, "Bolt Plume"),
    _SF(16, "Fire Fang"),
    _SF(16, "Antarctic Wind"),
    _SF(16, "Swift Bolt"),
    _SF(16, "Fire Veil"),
    _SF(16, "Ice Crystal"),
    _SF(16, "Megalixir"),
    _SF(16, "Turbo Ether"),
    _SF(16, "Potion"),
    _SF(16, "Phoenix Down"),
    _SF(16, "Hyper"),
    _SF(16, "Tranquilizer"),
    _SF(16, "Hi-Potion"),
};

u8 D_800B256C[][8] = {
    _SF(8, "SAM"),
    _SF(8, "ELEN"),
    _SF(8, "BLUES"),
    _SF(8, "TOM"),
    _SF(8, "JOHN"),
    _SF(8, "GARY"),
    _SF(8, "MIKE"),
    _SF(8, "SANDY"),
    _SF(8, "JU"),
    _SF(8, "LY"),
    _SF(8, "JOEL"),
    _SF(8, "GREY"),
    _SF(8, "EDWARD"),
    _SF(8, "JAMES"),
    _SF(8, "HARVEY"),
    _SF(8, "DAN"),
    _SF(8, "RUDY"),
    _SF(8, "GRAHAM"),
    _SF(8, "FOX"),
    _SF(8, "CLIVE"),
    _SF(8, "SEAN"),
    _SF(8, "YOUNG"),
    _SF(8, "ROBIN"),
    _SF(8, "DARIO"),
    _SF(8, "ARL"),
    _SF(8, "SARA"),
    _SF(8, "MARIE"),
    _SF(8, "SAMMY"),
    _SF(8, "LIA"),
    _SF(8, "KNIGHT"),
    _SF(8, "PAULA"),
    _SF(8, "PAU"),
    _SF(8, "LE"),
    _SF(8, "PETER"),
    _SF(8, "AIMEE"),
    _SF(8, "TERRY"),
    _SF(8, "ANDY"),
    _SF(8, "NANCY"),
    _SF(8, "TIM"),
    _SF(8, "ROBER"),
    _SF(8, "GEORGE"),
    _SF(8, "JENNY"),
    _SF(8, "RICA"),
    _SF(8, "JULIA"),
};
// clang-format on

#include "acos.h"
