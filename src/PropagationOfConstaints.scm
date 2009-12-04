;;
;; 约束的传播
;;
;; SICP中第3.3.5节描述的系统的实现
;;


;; 系统说明

;; 
;; 约束系统的实现
;;

(define list
  (lambda objs
    objs))

(define memq
  (lambda (obj list)
    (if (null? list)
	#f
	(if (eq? obj (car list))
	    list
	    (memq obj (cdr list))))))

(define error
  (lambda (message obj)
    (display "Error! ")
    (display message)
    (display " ")
    (display obj)
    (newline)))

;; 在被求和连接器a1和a2与和连接器sum之间构造出一个加法约束
(define adder
  (lambda (a1 a2 sum)
    (define process-new-value
      (lambda ()
	(if (and (has-value? a1) (has-value? a2))
	    (set-value! sum		;求和
			(+ (get-value a1) (get-value a2))
			me)
	    (if (and (has-value? a1) (has-value? sum))
		(set-value! a2		;求a2
			    (- (get-value sum) (get-value a1))
			    me)
		(set-value! a1		;求a1
			    (- (get-value sum) (get-value a2))
			    me)))))
    (define process-forget-value
      (lambda ()
	(forget-value! sum me)
	(forget-value! a1 me)
	(forget-value! a2 me)
	(process-new-value)))
    (define me
      (lambda (request)
	(if (eq? request 'I-have-a-value)
	    (process-new-value)
	    (if (eq? request 'I-lost-a-value)
		(process-forget-value)
		(error "Unknown request -- ADDER" request)))))
    (connect a1 me)
    (connect a2 me)
    (connect sum me)
    me))

;; 乘法约束
(define multiplier
  (lambda (m1 m2 product)
    (define process-new-value
      (lambda ()
	(if (or (and (has-value? m1) (= (get-value m1) 0))
		(and (has-value? m2) (= (get-value m2) 0)))
	    (set-value! product 0 me)
	    (if (and (has-value? m1) (has-value? m2))
		(set-value! product
			    (* (get-value m1) (get-value m2))
			    me)
		(if (and (has-value? product) (has-value? m1))
		    (set-value! product
				(/ (get-value product) (get-value m1))
				me)
		    (if (and (has-value? product) (has-value? m2))
			(set-value! product
				    (/ (get-value product) (get-value m2))
				    me)))))))
    (define process-forget-value
      (lambda ()
	(forget-value! product me)
	(forget-value! m1 me)
	(forget-value! m2 me)
	(process-new-value)))
    (define me
      (lambda (request)
	(if (eq? request 'I-have-a-value)
	    (process-new-value)
	    (if (eq? request 'I-lost-a-value)
		(process-forget-value)
		(error "Unknown request" request)))))
    (connect m1 me)
    (connect m2 me)
    (connect product me)
    me))



;; 常量约束
(define constant
  (lambda (value connector)
    (define me
      (lambda (request)
	(error "Unknown request -- CONSTANT" request)))
    (connect connector me)
    (set-value! connector value me)
    me))

;; 设置或取消值时打印出一个消息
(define probe
  (lambda (name connector)
    (define print-probe
      (lambda (value)
	(newline)
	(display "Probe: ")
	(display name)
	(display " = ")
	(display value)))
    (define process-new-value
      (lambda ()
	(print-probe (get-value connector))))
    (define process-forget-value
      (lambda ()
	(print-probe "?")))
    (define me
      (lambda (request)
	(if (eq? request 'I-have-a-value)
	    (process-new-value)
	    (if (eq? request 'I-lost-a-value)
		(process-forget-value)
		(error "Unknown request -- PROBE" request)))))
    (connect connector me)
    me))

;; 通知约束
(define inform-about-value
  (lambda (constraint)
    (constraint 'I-have-a-value)))

(define inform-about-no-value
  (lambda (constraint)
    (constraint 'I-lost-a-value)))


;; 连接器的表示
(define make-connector
  (lambda ()
    (define value false)
    (define informant false)
    (define constraints '())
    (define set-my-value
      (lambda (newval setter)
	(if (not (has-value? me))
	    (begin (set! value newval)
		   (set! informant setter)
		   (for-each-except setter
				    inform-about-value
				    constraints))
	    (if (not (= value newval))
		(error "Contradiction" (list value newval))
		'ignored))))
    (define forget-my-value
      (lambda (retractor)
	(if (eq? retractor informant)
	    (begin (set! informant false)
		   (for-each-except retractor
				    inform-about-no-value
				    constraints))
	    'ignored)))
    (define connect
      (lambda (new-constraint)
	(if (not (memq new-constraint constraints))
	    (set! constraints
		  (cons new-constraint constraints)))
	(if (has-value? me)
	    (inform-about-value new-constraint))
	'done))
    (define me
      (lambda (request)
	(if (eq? request 'has-value?)
	    (if informant true false)
	    (if (eq? request 'value)
		value
		(if (eq? request 'set-value!)
		    set-my-value
		    (if (eq? request 'forget)
			forget-my-value
			(if (eq? request 'connect)
			    connect
			    (error "Unknown operation -- CONNECTOR"
				   request))))))))
    me))

(define true #t)
(define false #f)

(define for-each-except
  (lambda (exception procedure list)
    (define loop
      (lambda (items)
	(if (null? items)
	    'done
	    (if (eq? (car items) exception)
		(loop (cdr items))
		(begin (procedure (car items))
		       (loop (cdr items)))))))
    (loop list)))



;; 基本操作

(define has-value?
  (lambda (connector)
    (connector 'has-value?)))

(define get-value
  (lambda (connector)
    (connector 'value)))

(define set-value!
  (lambda (connector new-value informant)
    ((connector 'set-value!) new-value informant)))

(define forget-value!
  (lambda (connector retractor)
    ((connector 'forget) retractor)))

(define connect
  (lambda (connector new-constraint)
    ((connector 'connect) new-constraint)))


;; 求温度关系的过程的定义：9C = 5(F - 32)
(define celsius-fahrenheit-converter
  (lambda (c f)
    (define u (make-connector))
    (define v (make-connector))
    (define w (make-connector))
    (define x (make-connector))
    (define y (make-connector))
    (multiplier c w u)
    (multiplier v x u)
    (adder v y f)
    (constant 9 w)
    (constant 5 x)
    (constant 32 y)
    'ok))

;; a+90=b+3
(define test-adder
  (lambda (a b)
    (define a1 (make-connector))
    (define a2 (make-connector))
    (define a3 (make-connector))
    (constant 90 a1)
    (constant 3 a2)
    (adder a a1 a3)
    (adder b a2 a3)
    'ok))

;; 
;; 约束系统的使用
;;

(define C (make-connector))
(define F (make-connector))
(celsius-fahrenheit-converter C F)



(probe "Celsius temp" C)
(probe "Fahrenheit temp" F)

(set-value! C 25 'user)


(define A (make-connector))
(define B (make-connector))
(test-adder A B)
(probe "Adder A" A)
(probe "Adder B" B)

(set-value! A 25 'user)