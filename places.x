
/*places.x*/
typedef struct placesnode* placeslist;

/* a node in the places list*/
struct placesnode{
       string code<>;
       string name<>;
       string state<2>;
       float latitude;
       float longitude;
       float distance;
       placeslist next;
};

struct location{
       string city<>;
       string state<2>;
       string host<>;
};


/*result of a readdir operation*/
union readplaces_ret switch (int err) {
      case 0:
           placeslist list; /*no error, return places listing*/
      case 1:
           void; /*error occured, nothing returned*/
};

/*IDL program*/
program PLACESPROG {
        version PLACES_VERS {
                readplaces_ret get_places(struct location) = 1;
                } = 1;
} = 0x3777777D;

