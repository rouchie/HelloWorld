#pragma once

#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>

template <typename T>
class CIndex {
	struct node {
		node(T _i) { i = _i; n = NULL; }
		T i;
		struct node *n;
	};
	typedef node list;
	public:
		static_assert(std::is_arithmetic<T>::value, "Not Support");

		CIndex() {
			h = NULL;
			value = 0;
		}

		virtual ~CIndex() {
			while (h != NULL) {
				node *n = h;
				h = h->n;
				delete n;
			}
		}

		T pop() {
			if (h) {
				node *t = h;
				T _i = h->i;
				h = h->n;
				delete t;
				return _i;
			}
			return value++;
		}

		void push(T _i) {
			if (_i == value-1) {
				value--;
				return ;
			}
			node *t = new node(_i);
			if (!h) {
				h = t;
			} else {
				if (t->i < h->i) {
					t->n = h;
					h = t;
				} else {
					node *k = h;
					while(k->n) {
						if (t->i < k->n->i) {
							t->n = k->n;
							k->n = t;
							break;
						}
						k = k->n;
					}
					if (!k->n)
						k->n = t;
				}
			}
		}

		void clear() {
			while (h != NULL) {
				node *n = h;
				h = h->n;
				delete n;
			}
			h = nullptr;
			value = 0;
		}
	private:
		struct node *h;
		T value;
};

