
;; 出错提示
(define error
  (lambda (message obj)
    (display "Error! ")
    (display message)
    (display " ")
    (display obj)
    (newline)))



;; 求a/b的余数
(define remainder
  (lambda (a b)
    (if (< a b)
	a
	(remainder (- a b) b))))
;; 最大公约数
;; 采用欧几里得算法实现，迭代方式
(define gcd
  (lambda (a b)
    (if (= b 0)
	a
	(gcd b (remainder a b)))))
;; 求幂
;; b^n=b*b^(n-1)
;; b^0=1

;; 递归版本
(define expt
  (lambda (b n)
    (if (= n 0)
	1
	(* b (expt b (- n 1))))))
;; 迭代版本
(define expt
  (lambda (b n)
    (define expt-iter
      (lambda (b n result)
	(if (= n 0)
	    result
	    (expt-iter b
		       (- n 1)
		       (* b result)))))
    (expt-iter b n 1)))
  


;;
;; 发现一个用法可以打印出一个复合对象的内部表示
;;
(define *show-obj-struct*
  (lambda obj
    obj))
;(*show-obj-struct* . obj)


;; 
;; 这里定义一些基本操作，以满足Scheme标准的要求
;;

;; ;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; R5RS 4.2
;; 派生表达式类型
;;
;; ;;;;;;;;;;;;;;;;;;;;;;;;;;



(define head
  (lambda (pair)
    (car pair)))

(define tail
  (lambda (pair)
    (cdr pair)))

;; 创建表
(define list (lambda items items))


;; cadr caddr cadddr ...
(define caar (lambda (pair) (car (car pair))))
(define cadr (lambda (pair) (car (cdr pair))))
(define cddr (lambda (pair) (cdr (cdr pair))))
(define cdar (lambda (pair) (cdr (car pair))))

(define caaar (lambda (pair) (caar (car pair))))
(define caadr (lambda (pair) (caar (cdr pair))))
(define cadar (lambda (pair) (cadr (car pair))))
(define caddr (lambda (pair) (cadr (cdr pair))))
(define cdddr (lambda (pair) (cddr (cdr pair))))
(define cddar (lambda (pair) (cddr (car pair))))
(define cdaar (lambda (pair) (cdar (car pair))))

(define caaaar (lambda (pair) (caaar (car pair))))
(define caaadr (lambda (pair) (caaar (cdr pair))))
(define caaddr (lambda (pair) (caadr (cdr pair))))
(define cadddr (lambda (pair) (caddr (cdr pair))))
(define cddddr (lambda (pair) (cdddr (cdr pair))))
(define cdddar (lambda (pair) (cdddr (car pair))))
(define cddaar (lambda (pair) (cddar (car pair))))
(define cdaaar (lambda (pair) (cdaar (car pair))))


;; (append list ...) - R5RS P24
(define append
  (lambda list
    (if (null? list)			;参数为空
	'()
	(if (null? (cdr list))		;参数只有一个
	    (car list)
	    (if (null? (cddr list))	;参数只有两个
		(if (null? (car list))
		    (cadr list)
		    (cons (caar list)
			  (append (cdar list) (cadr list))))
		(append (car list) (append (cdr list)))))))) ;参数多于两个

;; *mem-base*
(define *mem-base*
  (lambda (obj list pred)
    (if (null? list)
	#f
	(if (pred obj (car list))
	    list
	    (*mem-base* obj (cdr list) pred)))))

;; (memq obj list)
(define memq
  (lambda (obj list)
    (*mem-base* obj list eq?)))

;; (memv obj list)
(define memv
  (lambda (obj list)
    (*mem-base* obj list eqv?)))

;; (member obj list)
(define member
  (lambda (obj list)
    (*mem-base* obj list equal?)))

;; *ass-base*
(define *ass-base*
  (lambda (obj alist pred)
    ;; 
    (define ass-iter
      (lambda (first rest)
	(if (pred obj (car first))
	   first
	   (if (null? rest)
	       #f
	       (ass-iter (car rest)
			 (cdr rest))))))
    ;;
    (if (null? alist)
	#f
	(ass-iter (car alist) (cdr alist)))))
	
;; (assq obj alist)
(define assq
  (lambda (obj alist)
    (*ass-base* obj alist eq?)))
;; (assv obj alist)
(define assv
  (lambda (obj alist)
    (*ass-base* obj alist eqv?)))
;; (assoc obj alist)
(define assoc
  (lambda (obj alist)
    (*ass-base* obj alist equal?)))



