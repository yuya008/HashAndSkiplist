#ifndef _HASHANDSKIPLIST_H_
#define _HASHANDSKIPLIST_H_

#define TL				10000
#define maxHeight	10
#define id(x)			(x->ms->id)

typedef struct bucket Bucket;
typedef struct ms *Ms;
typedef struct ms_t *Mst;

struct bucket
{
	int maxLevel;
	Mst top;
	Mst tail;
};

struct ms_t
{
	Ms ms;
	int level;
	Mst next;
	Mst down;
};

struct ms
{
	uint64_t id;
	struct data *d;
};

int put(Ms ms);
Mst find(uint64_t id);

#endif
