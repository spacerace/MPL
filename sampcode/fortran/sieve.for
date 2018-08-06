	subroutine second (t1)
c
c  MS version of SECOND (timing routine)
c
	integer*2 ih,im,is,ihu
	integer*4 t1
	call gettim (ih,im,is,ihu)
	t1 = (ih*3600+im*60+is)*100+ihu
	end
*     prime number sieve program (integer*4 version)

      integer*4   i, niter, count, prime
	integer*4 t1,t2
c     integer   t1 (4), t2 (4)

c     write  (*, '  ('' no. iterations: '' )   ')
c     read   (*, *) niter
      niter = 10

c     call time (t1)
      call second (t1)
      do 30 i = 1, niter
         call sieve (count, prime)
30       continue

c     call etime (t2, t1, niter)
      call second (t2)
      write  (*, 40) count, prime
40    format ('0 done', I6, ' primes, largest is ', I6)
      write (*,*) 'Elapsed=',t2-t1,' sieve4 '
      write (*,*) 'Average per iteration=',+(t2-t1)/niter
      end

      subroutine sieve (count, largest)
      integer*4         count, largest

      integer*4     size
      parameter   (size = 8191)

      integer*4   i, prime, k
      logical     flags (size)

      count = 0
      do 10 i = 1, size
         flags (i) = .true.
10       continue

      do 30 i = 1, size
         if (flags (i)) then
            prime = i + i + 1
            do 20 k = i + prime, size, prime
               flags (k) = .false.
20             continue
            count = count + 1
            end if
30       continue
      largest = prime
      return
      end
