#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/time.h>
#include <fcntl.h>

#include "HashAndSkiplist.h"

static struct bucket hashTable[TL];

/**
 * 放入元素
 */
int put(Ms ms)
{
	if (!ms) {
		return 1;
	}

	int hash_value = ms->id % TL;
	Bucket *b = &hashTable[hash_value];

	if (_insertSkipList(b, ms)) {
		return 1;
	}
	return 0;
}

/**
 * 插入
 */
int _insertSkipList(Bucket *bu, Ms ms)
{
	if (find(ms->id)) {
		free(ms);
		return 0;
	}
	if (!bu->top || !bu->tail) {
		_initSkipList(bu);					// 初始化skiplist
	}

	int r = (rand() % maxHeight) + 1;		// 插入元素的随机层数
	if (r > bu->maxLevel) {					// 如果插入元素随机层数大于当前最大层数值，那么触发扩展skiplist
		extensionSkipList(bu, r);
	}

	Mst newlist = createList(r, ms);
	Mst top = bu->top;

	for(;;) {
		if (!top || !newlist) {
			break;
		}
		if (id(top->next) < id(newlist) && (top = top->next)) {
			continue;
		} else if (top->level > newlist->level && (top = top->down)) {
			continue;
		}

		if (top->level != newlist->level) {
			return 1;
		}

		newlist->next = top->next;
		top->next = newlist;

		newlist = newlist->down;
		top = top->down;
	}
	return 0;
}

void freeList(Mst list)
{
	for (;list;list = list->down) {
		free(list);
	}
}

Mst createList(int r, Ms ms)
{
	int i;
	Mst mst = NULL, t = NULL;

	for (i = 1; i <= r; i++) {
		mst = creatNode(ms);
		mst->level = i;
		if (t) {
			mst->down = t;
		}

		t = mst;
	}
	return mst;
}

void extensionSkipList(Bucket *bu, int r)
{
	if (bu->maxLevel >= r) {
		return;
	}

	int i = bu->maxLevel + 1;
	Mst h = NULL, t = NULL, e = bu->top,
			k = bu->tail;

	for (;i <= r;i++) {
		h = creatNode(bu->top->ms);
		t = creatNode(bu->tail->ms);

		h->level = t->level = i;
		h->next = t;
		h->down = e;
		t->down = k;
		e = h;
		k = t;
	}
	bu->top = e;
	bu->tail = k;
	bu->maxLevel = r;
}

/**
 * 初始化skiplist
 * 随机制造出最小值一列和最大值一列
 */
void _initSkipList(Bucket *bu)
{
	int r = (rand() % maxHeight) + 1, l = r;
	Mst t = NULL, e = NULL,tmp = NULL,
			k = NULL, v = NULL, m = NULL;

	Ms min = malloc(sizeof(struct ms));
	if (min == NULL) {
		fprintf(stderr, "%s\n", "OOM");
		exit(1);
	}
	Ms max = malloc(sizeof(struct ms));
	if (max == NULL) {
		fprintf(stderr, "%s\n", "OOM");
		exit(1);
	}
	min->id = 0;
	max->id = -1;

	for (;r;r--) {
		t = creatNode(min);
		tmp = creatNode(max);

		t->level = tmp->level = r;
		t->next = tmp;
		if (e && k) {
			e->down = t;
			k->down = tmp;
		} else {
			v = t;
			m = tmp;
		}
		e = t;
		k = tmp;
	}
	bu->top = v;
	bu->tail = m;
	bu->maxLevel = l;
}

/*
 * 创建链表节点
 */
Mst creatNode(Ms ms)
{
	Mst mst = NULL;

	if (!ms) {
		return NULL;
	}

	mst = malloc(sizeof(struct ms_t));

	if (mst == NULL) {
		fprintf(stderr, "%s\n", "OOM");
		exit(1);
	}

	mst->ms = ms;
	mst->level = 0;
	mst->next = mst->down = NULL;
	return mst;
}

uint64_t nanoseconds(void)
{
    struct timeval tv;
    if (gettimeofday(&tv, 0)) {
    	return -1;
    }
    return ((uint64_t)tv.tv_sec)*1000000000 + ((uint64_t)tv.tv_usec)*1000;
}

Mst find(uint64_t id)
{
	Bucket *b = &hashTable[id % TL];
	if (!b->top) {
		return NULL;
	}
	Mst t = b->top;
	for (;;) {
		if (!t) {
			break;
		}
		if (id(t) == id) {
			break;
		}
		if (t->next == NULL) {
			t = NULL;
			break;
		}
		if (id(t->next) == id) {
			t = t->next;
			break;
		} else if (id(t->next) < id) {
			t = t->next;
			continue;
		}
		if (t->down == NULL) {
			t = NULL;
			break;
		}
		t = t->down;
	}
	return t;
}
//
//	顺序写入1亿条数据，随机读取10000条
//
void doTest1()
{

#define ulonglong	(unsigned long long)

	uint64_t i,r;
	Ms ms = NULL;
	Mst mst = NULL;
	fprintf(stderr, "开始写入数据 %llu\n", ulonglong nanoseconds());
	for (i = 0; i < 100000000; i++)
	{
		ms = malloc(sizeof(struct ms));
		ms->id = i + 1;
		if(put(ms)) {
			fprintf(stdout, "写入 %llu 失败\n", ulonglong(i + 1));
			exit(1);
		} else {
			fprintf(stdout, "写入 %llu 成功\n", ulonglong(i + 1));

		}
	}
	fprintf(stderr, "写入完毕 %llu\n\n", ulonglong nanoseconds());

	for (i = 0; i < 10000; i++)
	{
		r = rand() % 100000000 + 1;

		fprintf(stderr, "开始查找 %llu", ulonglong r);
		fprintf(stderr, " %llu\n", ulonglong nanoseconds());
		mst = find(r);
		if (mst) {
			fprintf(stderr, "%llu 找到了 %llu\n", ulonglong r,
					ulonglong nanoseconds());
		} else {
			fprintf(stderr, "%llu 没找到 %llu\n", ulonglong r,
					ulonglong nanoseconds());
			continue;
		}
		fprintf(stderr, "id : %llu\n", ulonglong mst->ms->id);
		fprintf(stderr, "mst : %p\n", mst);
		fprintf(stderr, "ms : %p\n\n", mst->ms);
	}
}

void doTest2()
{
	int i,r;
	Ms ms = NULL;
	Mst mst = NULL;
	ssize_t s;
	int fd = open("tmp", O_RDWR|O_CREAT|O_TRUNC, 0777);

	for (i = 0; i < 10000000; i++)
	{
		r = rand() % 10000000 + 1;

		ms = malloc(sizeof(struct ms));
		ms->id = r;
		if(put(ms)) {
			fprintf(stderr, "%s\n", "插入失败");
			exit(1);
		}
		write(fd, &r, sizeof(int));
	}
	fprintf(stderr, "%s\n", "开始读");
	lseek(fd, 0, SEEK_SET);

	while ((s = read(fd, &r, sizeof(int))))
	{
		fprintf(stderr, "开始查找 %d", r);
		fprintf(stderr, " %llu\n", ulonglong nanoseconds());
		if ((mst = find(r))) {
			fprintf(stderr, "%llu 找到了 %llu\n", ulonglong r,
								ulonglong nanoseconds());
		} else {
			fprintf(stderr, "%llu 没找到 %llu\n", ulonglong r,
								ulonglong nanoseconds());
						continue;
		}
		fprintf(stderr, "Ms %p\n", mst->ms);
		fprintf(stderr, "Mst %p\n\n", mst);
	}
	close(fd);
}

int main(int argc, char **argv)
{
	//doTest1();		// 顺序写入1亿条数据，随机读取10000条
	doTest2();		// 顺序写入，随机读取
	return 0;
}
