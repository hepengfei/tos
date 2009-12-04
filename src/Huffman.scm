
;;
;; Huffman编码与解码
;;
;; 对给定的符号表进行编码与解码
;;
;; 主要操作：
;; count-freq - 根据符号表生成符号-频度表
;; generate-huffman-tree - 根据符号-频度表生成相应的Huffman树
;; encode - 按照指定的Huffman树，对符号表进行编码，生成0/1编码表
;; decode - 按照指定的Huffman树，对0/1编码表进行解码，还原出符号表
;;
;; TODO:
;; 解码操作的效率可能还会有较大的提升空间。
;; 除主要操作外，其它辅助操作全部实现为局部函数，以提高内聚性。
;;
;; 该代码在guile以及我自己的解释器tos::eval下调试通过。
;;
;; 2007-05-18 by kebing



;; 字母及频度表
;; 字母表及频度的表示
;; 
;; ( ('A 9) ('B 5) ('C 6) )


;; 给定字母表及频度，构造出Huffman树。
(define generate-huffman-tree
  (lambda (pairs)
    (successive-merge (make-leaf-set pairs))))

;; 将字母及频度表转换为有序的结点表
(define make-leaf-set
  (lambda (pairs)
    (if (null? pairs)
	'()
	(adjoin-set (make-leaf (caar pairs)
			       (cadar pairs))
		    (make-leaf-set (cdr pairs))))))

;; 将新结点加入有序表中，并保持表有序。有序是指从小到大。
(define adjoin-set
  (lambda (new set)
    (if (null? set)
	(list new)
	(if (< (weight new) (weight (car set)))
	    (cons new set)
	    (cons (car set) (adjoin-set new (cdr set)))))))

;; 将按权重从小到大排序的结点归并成树
(define successive-merge
  (lambda (leafs)
    (if (null? (cdr leafs))		;只剩一个结点，则为结果
	(car leafs)
	(successive-merge (adjoin-set (make-code-tree (car leafs)
						      (cadr leafs))
				      (cddr leafs))))))

;; 将叶子结点表示为(leaf 符号 权重)
(define make-leaf
  (lambda (symbol weight)
    (list 'leaf symbol weight)))

(define leaf?
  (lambda (object)
    (eq? 'leaf (car object))))

;; 获取符号
(define symbol-leaf
  (lambda (leaf)
    (cadr leaf)))

;; 获取权重
(define weight-leaf
  (lambda (leaf)
    (caddr leaf)))

;; 树表示为(left-leaf right-leaf symbol-set weight)
;; 其中symbol-set是所有子结点的符号的合并；weight是子结点权重的和

(define make-code-tree
  (lambda (left right)
    (list left
	  right
	  (append (symbols left) (symbols right))
	  (+ (weight left) (weight right)))))

(define left-branch
  (lambda (tree)
    (car tree)))

(define right-branch
  (lambda (tree)
    (cadr tree)))

(define symbols
  (lambda (tree)
    (if (leaf? tree)
	(list (symbol-leaf tree))
	(caddr tree))))

(define weight
  (lambda (tree)
    (if (leaf? tree)
	(weight-leaf tree)
	(cadddr tree))))


;; 解码，对给定的Huffman树和0/1码表
(define decode
  (lambda (bits tree)
    ;; 按照位来选取相应子树
    (define choose-branch
      (lambda (bit tree)
	(if (= bit 0)
	    (left-branch tree)
	    (if (= bit 1)
		(right-branch tree)
		(error "bad bit -- CHOOSE-BRANCH" bit)))))
    ;;
    (define decode-1
      (lambda (bits branch)
	(if (null? bits)
	    '()
	    ((lambda (next)
	       (if (leaf? next)
		   (cons (symbol-leaf next)
			 (decode-1 (cdr bits) tree))
		   (decode-1 (cdr bits) next)))
	     (choose-branch (car bits) branch)))))
    ;;
    (decode-1 bits tree)))

;; 根据给定的Huffman树，对符号表，产生出它的0/1编码表
;; 编码效率太差
(define encode
  (lambda (message tree)
    ;; 
    (define encode-symbol
      (lambda (symbol tree)
	;;
	(define search-symbol
	  (lambda (tree)
		(if (memq symbol (symbols tree))
		    (if (leaf? tree)
			'()
			((lambda (result)
			   (if result
			       (cons 0 result)
			       ((lambda (result)
				  (if result
				      (cons 1 result)
				      (begin (error "cannot find symbol in tree -- ENCODE" symbol)
					     #f)))
				(search-symbol (right-branch tree)))))
			   (search-symbol (left-branch tree))))
		    #f)))
	(search-symbol tree)))
    ;; 
    (if (null? message)
	'()
	(append (encode-symbol (car message) tree)
		(encode (cdr message) tree)))))

;; 再添加一个统计符号频率，并生成符号频度表的函数即完美了
(define count-freq
  (lambda (symbol-list)
    ;; 
    (define count-iter
      (lambda (result rest)
	;;
	(define deal
	  (lambda (cur-found)
	    (if cur-found
		(begin (inc-freq! cur-found)
		       result)
		(add-new-symbol (car rest) result))))
	;;
	(if (null? rest)
	    result
	    (count-iter (deal (assq (car rest) result))
			(cdr rest)))))
    ;;
    (define add-new-symbol
      (lambda (symbol pairs)
	(cons (list symbol 1) pairs)))
    ;;
    (define inc-freq!
      (lambda (item)
	(set-car! (cdr item) (+ 1 (cadr item)))))
    ;; 
    (count-iter '() symbol-list)))




;; 测试Huffman树的代码

(define test-huffman
  (lambda ()
    (newline)
    (display "input a symbol list:")
    (define message (read))

    (if (or (eof-object? message)
	    (equal? message 'quit))
	'quit
	(if (not (list? message))
	    (begin
	      (display "please input a list.")
	      (newline)
	      (test-huffman))
	    (begin
	      (display "counting frequence...")
	      (newline)
	      (define freq-pairs (count-freq message))
	      (display freq-pairs)
	      (newline)
	      
	      (display "building huffman tree...")
	      (newline)
	      (define huffman-tree (generate-huffman-tree freq-pairs))
	      (display huffman-tree)
	      (newline)

	      (display "encoding message...")
	      (newline)
	      (define bits (encode message huffman-tree))
	      (display bits)
	      (newline)

	      (display "recovering message from bits...")
	      (newline)
	      (define recover (decode bits huffman-tree))
	      (display recover)
	      (test-huffman))))))


;; 启动测试程序
(test-huffman)
