#include <urlfor/routes.h>

struct Route our_routes = {
    .cb = Route_stdout,
};

void Route_stdout(bstring entry) {
    _unused(entry);
    debug("<html> Entry: %s </html>", bdata(entry));
}
