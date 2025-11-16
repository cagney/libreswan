
#define linked_list(LIST, DATA)			\
	LIST {					\
		LIST *prev;			\
		LIST *next;			\
		DATA data;			\
	}

#define list_append(LIST, OBJECT)					\
	{								\
		typeof(LIST) new_ = alloc_thing(typeof(*(LIST)), #OBJECT); \
		new_->data = OBJECT;					\
		if ((LIST) == NULL) {					\
			(LIST) = new_->prev = new_->next = new_;	\
		} else {						\
			new_->prev = (LIST)->prev;			\
			new_->next = (LIST);				\
			(LIST)->prev->next = new_;			\
			(LIST)->prev = new_;				\
		}							\
	}
