#include <assert.h>
#include <stdbool.h>

typedef struct
{
    bool (*a)();
} j;
bool b() { return 1; }

int main()
{
    j actions[1];
    actions[0] = (j){b};
    int i = 0;

    bool y = (actions + i)->a();
    y = (actions + 0)->a();

    assert(y == 1);
}
// Imported from ESBMC regression/esbmc/github_2292_5
// EXPECT: ^VERIFICATION SUCCESSFUL$
