#ifdef USE_SENSEI

c...==============================================================
c... subroutine to do the coprocessing
c... The subroutine is responsible for determining if coprocessing
c... is needed this timestep and if it is needed then the
c... subroutine passes the phasta data structures into
c... the coprocessor. This is meant to be called at the end of
c... every time step.
c... The input is:
c...    itimestep -- the current simulation time step
c...    X -- the coordinates array of the nodes
c...    Y -- the fields array (e.g. velocity, pressure, etc.)
c...    compressibleflow -- flag to indicate whether or not the
c...                         flow is compressible.  if it is then
c...                         temperature will be outputted
c...    computevort -- flag to indicate whether or not vorticity is computed
c...    VORTICITY -- the vorticity array
c... It has no output and should not change any Phasta data.
c...==============================================================

      subroutine ph_sensei_coprocess(itimestep, X, Y, compressibleflow,
     &                      computevort, VORTICITY, dwal, irank2ybar, ybar, surfid, 
     &                      vmeanLow, vampLow, vmeanUp, vampUp, 
     &                      upperBlowerPosition, ybarDummyWeight)
      use pointer_data
      include "common.h"
      integer iblk, nenl, npro, j, needflag, i
      integer compressibleflow, itimestep, computevort
      dimension x(numnp,nsd), y(nshg,ndof), vorticity(nshg, 5)
      dimension dwal(nshg)
      dimension ybar(nshg, irank2ybar)
      dimension surfid(nshg)
!      dimension ycontainer(nshg,ndof)
      if(docoprocessing .ne. 1) then
        return
      endif

      ! supplemental fields
      ! call addsurfids(surfid)
      ! call addfields(nshg, ndof, ycontainer, compressibleflow)

      if(myrank.eq.0)  then
         tcorecp5 = TMRC()
      endif
      call sensei_coprocess(itimestep)
      if(myrank.eq.0)  then
          tcorecp6 = TMRC()
          write(6,*) 'coprocess: ',tcorecp6-tcorecp5
      endif
      return
      end

      subroutine ph_sensei_init(X, Y, compressibleflow, vmeanLow, vampLow, 
     &                      vmeanUp, vampUp, upperBlowerPosition, 
     &                      ybarDummyWeight, dwal, ybar)
!       FIXME - using the old code for now - make configurable
!       and fix variable allocation
        use pointer_data
        include "common.h"
        dimension x(numnp,nsd), y(nshg,ndof)

        if(docoprocessing .ne. 1) then
          return
        endif
        if(nshg .ne. numnp) then
           print *, 'CoProcessing only setup for when nshg equals numnp'
           return
        endif

        docoprocessing = 1
        call sensei_adaptors_init() ! Provide XML filename here... or not.
        call createpointsandallocatecells(nsd, numnp, X, numel)
        do iblk=1,nelblk
            nenl = lcblk(5,iblk) ! no. of vertices per element
            npro = lcblk(1,iblk+1) - lcblk(1,iblk) ! no. of elemens in block
            call insertblockofcells(npro, nenl, mien(iblk)%p(1,1))
        enddo
        call addfields(nshg, ndof, Y, compressibleflow, vmeanLow, vampLow,
     &              vmeanUp, vampUp, upperBlowerPosition,
     &              ybarDummyWeight, dwal, ybar)

      end


      subroutine ph_sensei_fini()
        call sensei_adaptors_fini()
      end

#endif
