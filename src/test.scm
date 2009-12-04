;; 测试尾递归
;; 保存两种情况的栈深度变化数据
(define stack-deep '(0))
(define stack-deep-iter '(0))

;; 递归版本
(define expt
  (lambda (b n)
    (set! stack-deep
	  (append stack-deep
		  (list (length (tos::register 'stack)))))
    (if (= n 0)
	1
	(* b (expt b (- n 1))))))
;; 迭代版本
(define expt-iter
  (lambda (b n result)
    (set! stack-deep-iter
	  (append stack-deep-iter
		  (list (length (tos::register 'stack)))))
    (if (= n 0)
	result
	(expt-iter b
		   (- n 1)
		   (* b result)))))



;; 测试Huffman树的代码

(define test-huffman
  (lambda ()
    (newline)
    (display "input a symbol list:")
    (define message (read))

    (if (or (not (pair? message))
	    (equal? message 'quit)))
	'quit
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
	  (test-huffman)))))


(define encode lambda message tree define encode-symbol lambda symbol tree define search-symbol lambda tree if memq symbol symbols tree if leaf? tree lambda result if result cons 0 result lambda result if result cons 1 result begin error cannot find symbol in tree -- ENCODE symbol search-symbol right-branch tree search-symbol left-branch tree search-symbol tree if null? message append encode-symbol car message tree encode cdr message tree)


(d e f i n e e n c o d e l a m b d a m e s s a g e t r e e d e f i n e e n c o d e s y m b o l l a m b d a s y m b o l t r e e d e f i n e s e a r c h s y m b o l l a m b d a t r e e i f m e m q s y m b o l s y m b o l s t r e e i f l e a f t r e e l a m b d a r e s u l t i f r e s u l t c o n s 0 r e s u l t l a m b d a r e s u l t i f r e s u l t c o n s 1 r e s u l t b e g i n e r r o r c a n n o t f i n d s y m b o l i n t r e e E N C O D E s y m b o l s e a r c h s y m b o l r i g h t b r a n c h t r e e s e a r c h s y m b o l l e f t b r a n c h t r e e s e a r c h s y m b o l t r e e i f n u l l m e s s a g e a p p e n d e n c o d e s y m b o l c a r m e s s a g e t r e e e n c o d e c d r m e s s a g e t r e e)

(define message '(s c h e m e l a m b d a))
(define freq-pairs (count-freq message))
(define huffman-tree (generate-huffman-tree freq-pairs))
;; (1 1 1 0 1 1 1 1 0 0 0 1 0 0 1 0 1 1 0 0 0 0 1 1 1 0 1 0 1 0 1 0 0 1 1 1 1 0)
(define bits (encode message huffman-tree))
(define msg (decode bits huffman-tree))

(define sample-message '(A C A B B D A))
(define sample-code '(0 1 1 0 0 1 0 1 0 1 1 1 0))
(define sample-pairs '((A 4) (B 2) (C 1) (D 1)))
(define sample-tree (generate-huffman-tree sample-pairs))
(decode sample-code sample-tree)	;(a d a b b c a)
(encode '(A D A B B C A) sample-tree)	;(0 1 1 0 0 1 0 1 0 1 1 1 0)
(encode sample-message sample-tree)	;(0 1 1 1 0 1 0 1 0 1 1 0 0)

;(define pairs '((A 8) (B 3) (C 1) (D 1) (E 1) (F 1) (G 1) (H 1)))
;(define code-tree (generate-huffman-tree pairs))
;code-tree
(encode sample-message sample-tree)

	


;; ;;;;;;;;;;;;;;;;;;
;; P26 换零钱方式的统计
;;
;; 将总数为a的现金换成n种硬币的不同方式的数目：
;; 若a等于0,就是有1种换零钱的方式。
;; 若a小于0,就是有0种换零钱的方式。
;; 若n等于0,就是有0种换零钱的方式。
;;

(define coins
  (lambda (n)
    (define coins-vector #(1 5 10 25 50 100 500 1000))
    (vector-ref coins-vector (- n 1))))

(define count-change
  (lambda (a n)
    (if (= a 0) 1
	(if (or (< a 0) (= n 0))
	    0
	    (+ (count-change a (- n 1))
	       (count-change (- a (coins n)) n))))))

(count-change 30 4)




;; 创建表
(define list (lambda items items))

(define map
  (lambda (proc items)
    (if (null? items)
	'()
	(cons (proc (car items))
	      (map proc (cdr items))))))
		    

(define square (lambda (n) (* n n)))

;; 过程square-list以一个数值表为参数，返回每个数的平方构成的表
(define square-list
  (lambda (items)
    (if (null? items)
	'()
	(cons (square (car items))
	      (square-list (cdr items))))))

(define square-list
  (lambda (items)
    (map square items)))

(square-list (list 1 2 3 4 5 6 7 8 9))






;; TODO:

;; 输入输出端口
;; display newline write read
;; input-port? ...
(display "hello,\"ke\'bing\"")
(display #\a)
(display 23)
(display #f)
(display '(a #\b "abc"))			;这一条不符合语义，表内的字符串
					;和字符也要特殊处理。

(newline)

(write "hello,\"ke\'bing\"")
(write #\a)
(write 23)
(write #f)
(write '(a #\b "abc"))

(read)
;; 求值
;; eval
;; scheme-report-environment interaction-environment
(eval (read) (tos::register 'env))
(define a '(display "I can run."))
(eval a (tos::register 'env))

(define eval-loop
  (lambda ()
    (display "Please input:")
    (define tmp (read))
    (if (not (equal? tmp "quit"))
	(begin
	  (write (eval tmp (tos::register 'env)))
	  (newline)
	  (eval-loop))
	(display "Bye."))))
	
    
;; 控制特征
;; procedure? apply call-with-current-continuation
;; values call-with-values dynamic-wind
;;
;; map for-each force delay 

;; 向量
;; vector? make-vector? vector-length vector-ref
;; vector-set! 
;;
;; vector

;; 字符串
;; string? make-string string-length string-ref
;; string-set! 
;;
;; string

;; 字符
;; char? char=? ... char>=? char->integer integer->char
;; 

;; 符号操作
;; symbol? symbol->string string->symbol

;; 表操作
;; cons car cdr set-car! set-cdr! 
;;
;; pair? null? list?
;;
;; caar cadr ... cddddr list length append
;; reverse list-tail list-ref memq memv member
;; assq assqv assoc

(if (and (equal? (append) '())
	 (equal? (append '(a b c)) '(a b c))
	 (equal? (append '(x) '(y)) '(x y))
	 (equal? (append '(a) '(b c d)) '(a b c d))
	 (equal? (append '(a (b)) '((c))) '(a (b) (c)))
	 (equal? (append '(a b) '(c . d)) '(a b c . d))
	 (equal? (append '() 'a) 'a)
	 (equal? (append '(a b c) 'd) '(a b c . d)))
    'ok
    'failed)

(pair? '(a . b))
(pair? '(a b c))
(pair? '())
(pair? '#(a b))

;; 布尔操作
;; not boolean?

(define test-boolean-ops
  (lambda ()
    (if (and (eq? (not #t) #f)
	     (eq? (not #f) #t)
	     (eq? (not #\a) #f)
	     (boolean? #t)
	     (not (boolean? #\a))
	     (boolean? (not #())))
	'test-boolean-ops-ok
	'test-boolean-ops-failed)))


;; 谓词比较
;; eqv? eq? equal?
(define test-eqv
  (lambda ()
    (if (and (eqv? 'a 'a)
	     (not (eqv? 'a 'b))
	     (eqv? 2 2)
	     (eqv? '() '())
	     (not (eqv? (cons 1 2) (cons 1 2)))
	     (not (eqv? (lambda () 1)
			(lambda () 2)))
	     (not (eqv? #f '()))
	     (not (eqv? (lambda (a) a)
			(lambda (a) a)))
	     (begin (define p (lambda (x) x))
		    (eqv? p p)))
	'test-eqv?-ok
	'test-eqv?-failed)))

(define test-eq
  (lambda ()
    (if (and (eq? 'a 'a)		;#t
	     (not (eq? '(a) '(a)))	;#f未定义
	     (not (eq? (list 'a)
		       (list 'a)))	;#f
	     (not (eq? "a" "a"))	;#f未定义
	     (not (eq? "" ""))		;#f未定义
	     (not (eq? #() #()))	;#f未定义
	     (eq? '() '())		;#t
	     (eq? 2 2)			;#t未定义
	     (eq? #\A #\A)		;#t未定义
	     (eq? car car)		;#t
	     (begin (define n (+ 2 3))
		    (eq? n n))		;#t未定义
	     (begin (define x '(a))
		    (eq? x x))		;#t
	     (begin (define x '#())
		    (eq? x x))		;#t
	     (begin (define p (lambda (x) x))
		    (eq? p p)))		;#t
	'test-eq?-ok
	'test-eq?-failed)))

(define test-equal
  (lambda ()
    (if (and (equal? 'a 'a)		;#t
	     (equal? '(a) '(a))		;#t
	     (equal? '(a (b) c)
		     '(a (b) c))	;#t
	     (equal? "abc" "abc")	;#t
	     (equal? 2 2)		;#t
	     (equal? (make-vector 5 'a)
		     (make-vector 5 'a)) ;#t
	     (not (equal? (lambda (x) x)
			  (lambda (y) y))) ;#f未定义
	     (not (equal? (lambda (x) x)
		     (lambda (x) x))))	;#f未定义
	'test-equal?-ok
	'test-equal?-failed)))

 
;; 测试数学运算操作
;; + - * / = < > <= >= number? integer?
;; number->string string->number 
;; 将内置实现：
;;  quotient remainder
;; modulo exp log sin cos tan asin acos atan sqrt
;; expt
;; 外部定义：
;; zero? positive? negative? odd? even? max min
;; abs gcd lcm

(define test-number-ops
  (lambda ()
    (if (and (number? 43)
	     (not (number? #\a))
	     (integer? +43)
	     (not (integer? #\A))
	     (= 1 1 1 1 1)
	     (not (= 1 2 3 4 5))
	     (> 5 4 3 2 1)
	     (not (> 5 4 4 3 2))
	     (< 1 2 3 4 5)
	     (not (< 1 2 3 3 5))
	     (<= 1 1 2 2 3 3 5)
	     (>= 5 3 3 2 2 1)
	     (not (<= 1 1 2 3 2 5))
	     (not (>= 5 3 3 1 2 1))
	     (eq? (+ 2 (- 9 3 9) 2 (+) (- 1)) 0)
	     (eq? (* 5 9 (+ 4 6)) (- (/ 1000 2) (* 5 10)))
	     (equal? (number->string 43) "43")
	     (equal? (number->string 127 2) "#b1111111")
	     (equal? (number->string 127 10) "127")
	     (equal? (number->string 127 8) "#o177")
	     (equal? (number->string 127 16) "#x7f")
	     (equal? (string->number "123") 123)
	     (equal? (string->number "123" 10) 123)
	     (equal? (string->number "20" 16) 32)
	     (equal? (string->number "#b1111") 15)
	     (equal? (string->number "#o17") 15)
	     (equal? (string->number "#d15") 15)
	     (equal? (string->number "#xf") 15))
	'test-number-ops-ok
	'test-number-ops-failed)))
	     
(+)
(+ 1 3 4 5)
(*)
(* 2 4 6 8)

(- 2)
(- 8 3)
(- 9 1 2 3)
(/ 35 7)
(/ 10)

(+ (* 1 2 3) (- 6 5 4) (/ 100 25) 9)

;; 求a/b的余数
(define remainder
  (lambda (a b)
    (if (< a b)
	a
	(remainder (- a b) b))))
;; 最大公约数
(define gcd
  (lambda (a b)
    (if (= b 0)
	a
	(gcd b (remainder a b)))))


;; 基本语法操作测试
;; quote lambda if set! define begin and or
;; 未实现：cond case else do delay quasiquote

(and (= 2 2) (> 2 1))			;#t
(and (= 2 2) (< 2 1) a b)		;#f
(and 1 2 'c '(f g))			;(f g)
(and)					;#t


(or (= 2 2) (> 2 1))			;#t
(or (= 2 2) (< 2 1))			;#t
(or #f #f #f #f)			;#f
(or '(a b c) (/ 3 0))			;(a b c)

(begin (define a 9)
       (set! a 0)
       (+ a 1))
a


;; 测试对复合过程各种参数形式的调用

(define test-lambda-formal
  (lambda ()

    ;; 固定数量的参数形式
    (define fixed-args
      (lambda (take fixed number of arguments)
       (cons take
	     (cons fixed
		   (cons number
			 (cons of
			       (cons arguments
				     ())))))))

    ;; 任意数量形式的参数
    (define any-args
      (lambda variable
       (cons 'this
	     (cons 'form
		   (cons 'can
			 variable)))))

    ;; 非严格表形式的参数，n个以个的参数（至少n个）
    (define n-or-more-args
      (lambda (var1 var2 . varn)
       (cons 'this
	     (cons var1
		   (cons var2
			 varn)))))

    ;; 正确的调用
    (fixed-args 'take
		'fixed
		'number
		'of
		'arguments)
    
    (any-args)
    (any-args 'one)
    (any-args 'take
	      'any
	      'number
	      'of
	      'arguments)

    (n-or-more-args 'just 'fit)
    (n-or-more-args 'formal
		    'is
		    'the
		    'improper
		    'list)

    ;; 错误的调用，参数或多或少，这时会有出错提示
    #|
    (fixed-args 'too 'less)
    (fixed-args 'this
		'args
		'is
		'too
		'long
		'for
		'fixed-args)

    (n-or-more-args 'less)
    (n-or-more-args 'this
		    'args
		    'is
		    'too
		    'long
		    'for
		    'n-or-more-args)
    |#

    'ok))


(define test-all
  (lambda ()
    (test-lambda-formal)
    ;; other test
    ;; ...
    
    'ok
    ))

;; 运行测试
(test-all)
 