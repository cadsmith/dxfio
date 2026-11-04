import { useRef } from 'react'
import { CheckIcon } from 'lucide-react'
import { gsap, moving, useGSAP } from '../motion'

const releases = [
  { name: 'R10', code: 'AC1006', year: 1988 },
  { name: 'R12', code: 'AC1009', year: 1992 },
  { name: 'R13', code: 'AC1012', year: 1994 },
  { name: 'R14', code: 'AC1014', year: 1997 },
  { name: 'R2000', code: 'AC1015', year: 1999 },
  { name: 'R2004', code: 'AC1018', year: 2003 },
  { name: 'R2007', code: 'AC1021', year: 2006 },
  { name: 'R2010', code: 'AC1024', year: 2009 },
  { name: 'R2013', code: 'AC1027', year: 2012 },
  { name: 'R2018', code: 'AC1032', year: 2017 },
]

const eras = [
  { label: 'Text in a code page', from: 1, to: 7, tone: 'gold' },
  { label: 'Text in UTF-8', from: 7, to: 11, tone: 'blue' },
  { label: 'Entities and tables', from: 1, to: 3, tone: 'plain' },
  { label: 'Objects, classes and handles', from: 3, to: 11, tone: 'plain' },
]

export function Timeline() {
  const scope = useRef<HTMLDivElement>(null)
  useGSAP(
    () => {
      const media = gsap.matchMedia()
      media.add(moving, () => {
        const timeline = gsap.timeline({ scrollTrigger: { trigger: scope.current, start: 'top 75%', once: true } })
        timeline
          .set('.timeline-fill', { scale: 0 })
          .set('.grip-on', { scale: 0 })
          .from('.timeline-point', { opacity: 0, y: 8, duration: 0.35, stagger: 0.03, ease: 'power2.out', clearProps: 'all' })
        const points = gsap.utils.toArray<HTMLElement>('.timeline-point', scope.current)
        points.forEach((point, index) => {
          const reached = 0.3 + index * 0.12
          timeline.to(point.querySelector('.grip-on'), { scale: 1, duration: 0.3, ease: 'back.out(2.2)' }, reached)
          if (index < points.length - 1) {
            timeline.to(point.querySelector('.timeline-fill'), { scale: 1, duration: 0.12, ease: 'none' }, reached)
          }
        })
        timeline.from('.era', { opacity: 0, y: 6, duration: 0.45, stagger: 0.06, ease: 'power3.out' }, 0.3 + points.length * 0.12)
      })
      return () => media.revert()
    },
    { scope },
  )
  return (
    <div className="timeline" ref={scope}>
      <ol className="timeline-points">
        {releases.map((one) => (
          <li key={one.name} className="timeline-point">
            <strong>{one.name}</strong>
            <span className="timeline-node" aria-hidden="true">
              <span className="timeline-link">
                <span className="timeline-fill" />
              </span>
              <span className="grip">
                <span className="grip-on">
                  <CheckIcon strokeWidth={3} />
                </span>
              </span>
            </span>
            <span className="timeline-year">{one.year}</span>
            <code>{one.code}</code>
          </li>
        ))}
      </ol>
      <div className="eras">
        {eras.map((one) => (
          <span key={one.label} className={`era era-${one.tone}`} style={{ gridColumn: `${one.from} / ${one.to}` }}>
            {one.label}
          </span>
        ))}
      </div>
      <span className="hatch-name" aria-hidden="true">
        HATCH ANSI31
      </span>
    </div>
  )
}
