#ifndef _HASHANDSKIPLIST_H_
#define _HASHANDSKIPLIST_H_

#define TL			10000
#define maxHeight	10
#define id(x)		(x->ms->id)

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
int _insertSkipList(Bucket *bu, Ms);
void _initSkipList(Bucket *bu);
Mst creatNode(Ms ms);
void extensionSkipList(Bucket *bu, int r);
Mst createList(int,Ms);
void freeList(Mst list);
Mst find(uint64_t id);

#endif
