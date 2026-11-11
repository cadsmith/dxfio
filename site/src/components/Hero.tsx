import { useRef, useState } from 'react'
import { RotateCcwIcon } from 'lucide-react'
import { gsap, moving, useGSAP } from '../motion'

type Step = { command: string; prompts: string[]; record: [string, string][] }

const steps: Step[] = [
  {
    command: 'LINE',
    prompts: ['Specify first point: 5,15', 'Specify next point: 5,4'],
    record: [['0', 'LINE'], ['8', 'STROKE'], ['10', '5'], ['20', '15'], ['11', '5'], ['21', '4']],
  },
  {
    command: 'CIRCLE',
    prompts: ['Specify center point: 14.75,9.5', 'Specify radius: 5.5'],
    record: [['0', 'CIRCLE'], ['8', 'STROKE'], ['10', '14.75'], ['20', '9.5'], ['40', '5.5']],
  },
  {
    command: 'POINT',
    prompts: ['Specify a point: 5,20.5'],
    record: [['0', 'POINT'], ['8', 'GRIP'], ['10', '5'], ['20', '20.5']],
  },
]

const summary: [string, string][] = [
  ['0', 'LINE'],
  ['0', 'CIRCLE'],
  ['0', 'POINT'],
  ['8', 'STROKE, GRIP'],
]

const span = Array.from({ length: 61 }, (_, index) => index - 18)

const grips: [number, number][] = [
  [5, 9],
  [5, 20],
  [14.75, 14.5],
  [14.75, 9],
  [20.25, 14.5],
  [14.75, 20],
  [9.25, 14.5],
]

export function Stage() {
  const scope = useRef<HTMLDivElement>(null)
  const [run, setRun] = useState(0)
  // The finished record, as a prerendered page and a calm visitor see it; the intro clears it first.
  const [record, setRecord] = useState<[string, string][]>(summary)
  useGSAP(
    () => {
      const media = gsap.matchMedia()
      media.add(moving, () => {
        gsap.set('.pickbox', { svgOrigin: '0 0', smoothOrigin: false })
        gsap.set('.dot', { svgOrigin: '5 3.5', smoothOrigin: false })
        const timeline = gsap.timeline({ delay: 0.6, defaults: { ease: 'power2.inOut' } })
        const type = (text: string, at?: string | number) =>
          timeline.to('.prompt', { duration: Math.max(0.35, text.length * 0.035), text: { value: text }, ease: 'none' }, at)
        const click = () =>
          timeline.fromTo('.pickbox', { scale: 1.8 }, { scale: 1, duration: 0.25, ease: 'back.out(3)', immediateRender: false })
        timeline
          .set('.stem, .bowl, .dot', { opacity: 0 })
          .set('.band', { opacity: 0 })
          .set('.select rect', { opacity: 0 })
          .set('.cursor', { x: 26, y: 26, opacity: 0 })
          .set('.prompt', { text: { value: '' } })
          .call(() => { setRecord([]) })
          .from('.grid line', { opacity: 0, duration: 0.8, stagger: { each: 0.008, from: 'center' } })
          .to('.cursor', { opacity: 1, duration: 0.3 }, '-=0.3')
        type('LINE')
        type(steps[0].prompts[0], '+=0.15')
        timeline.to('.cursor', { x: 5, y: 20, duration: 0.8 }, '<')
        click()
        timeline
          .set('.band-line', { attr: { x1: 5, y1: 20, x2: 5, y2: 20 }, opacity: 1 })
        type(steps[0].prompts[1], '+=0.05')
        timeline
          .to('.cursor', { y: 9, duration: 0.7 }, '<')
          .to('.band-line', { attr: { y2: 9 }, duration: 0.7 }, '<')
        click()
        timeline
          .to('.stem', { opacity: 1, duration: 0.25 })
          .set('.band-line', { opacity: 0 })
          .call(() => { setRecord(steps[0].record) })
        type('CIRCLE', '+=0.35')
        type(steps[1].prompts[0], '+=0.15')
        timeline.to('.cursor', { x: 14.75, y: 14.5, duration: 0.8 }, '<')
        click()
        timeline
          .set('.band-radius', { attr: { x1: 14.75, y1: 14.5, x2: 14.75, y2: 14.5 }, opacity: 1 })
          .set('.band-circle', { attr: { r: 0 }, opacity: 1 })
        type(steps[1].prompts[1], '+=0.05')
        timeline
          .to('.cursor', { x: 20.25, duration: 0.8 }, '<')
          .to('.band-radius', { attr: { x2: 20.25 }, duration: 0.8 }, '<')
          .to('.band-circle', { attr: { r: 5.5 }, duration: 0.8 }, '<')
        click()
        timeline
          .to('.bowl', { opacity: 1, duration: 0.25 })
          .set('.band-radius, .band-circle', { opacity: 0 })
          .call(() => { setRecord(steps[1].record) })
        type('POINT', '+=0.35')
        type(steps[2].prompts[0], '+=0.15')
        timeline.to('.cursor', { x: 5, y: 3.5, duration: 0.8 }, '<')
        click()
        timeline
          .fromTo('.dot', { opacity: 0, scale: 0 }, { opacity: 1, scale: 1, duration: 0.45, ease: 'back.out(2.6)' })
          .call(() => { setRecord(steps[2].record) })
          .to('.cursor', { opacity: 0, duration: 0.4 }, '+=0.3')
          .to('.select rect', { opacity: 1, duration: 0.2, stagger: 0.04 }, '<')
          .to('.select rect', { opacity: 0, duration: 0.5 }, '+=0.9')
        type('', '<')
      })
      return () => { media.revert() }
    },
    { scope, dependencies: [run], revertOnUpdate: true },
  )
  return (
    <div className="stage" ref={scope}>
      <svg viewBox="-8 -6 40 36" preserveAspectRatio="xMidYMid slice" aria-hidden="true">
        <defs>
          <radialGradient id="fade">
            <stop offset="0.35" stopColor="#fff" />
            <stop offset="1" stopColor="#fff" stopOpacity="0" />
          </radialGradient>
          <mask id="fade-mask">
            <rect x="-30" y="-30" width="90" height="90" fill="url(#fade)" />
          </mask>
        </defs>
        <g className="grid" mask="url(#fade-mask)">
          {span.map((at) => (
            <g key={at} className={at % 6 === 0 ? 'major' : 'minor'}>
              <line x1={at} y1={-30} x2={at} y2={60} />
              <line x1={-30} y1={at} x2={60} y2={at} />
            </g>
          ))}
        </g>
        <g className="ink" fill="none" strokeWidth="2" strokeLinecap="round">
          <path className="stem" d="M5 9 V20" />
          <circle className="bowl" cx="14.75" cy="14.5" r="5.5" />
        </g>
        <rect className="dot" x="2.75" y="1.25" width="4.5" height="4.5" rx="0.9" strokeWidth="0.9" />
        {/* The rubber bands start as points, which the intro's revert sets them back to. */}
        <g className="band" fill="none">
          <line className="band-line" x1={0} y1={0} x2={0} y2={0} />
          <line className="band-radius" x1={0} y1={0} x2={0} y2={0} />
          <circle className="band-circle" cx="14.75" cy="14.5" r={0} />
        </g>
        <g className="select">
          {grips.map(([x, y]) => (
            <rect key={`${x} ${y}`} x={x - 0.55} y={y - 0.55} width="1.1" height="1.1" />
          ))}
        </g>
        <g className="cursor">
          <line x1={-60} y1={0} x2={60} y2={0} />
          <line x1={0} y1={-60} x2={0} y2={60} />
          <rect className="pickbox" x={-0.6} y={-0.6} width={1.2} height={1.2} />
        </g>
      </svg>
      <div className="command">
        <p className="command-line">
          <span className="command-label">Command:</span> <span className="prompt" />
          <span className="caret" />
        </p>
        <p className="command-record" aria-live="polite">
          {record.length > 0 ? (
            record.map(([code, value]) => (
              <span key={`${code} ${value}`}>
                <b>{code}</b> {value}
              </span>
            ))
          ) : (
            <span className="faint">the entity's DXF record appears here</span>
          )}
        </p>
      </div>
      <button type="button" className="stage-replay" onClick={() => { setRun((one) => one + 1) }} aria-label="Draw the mark again">
        <RotateCcwIcon />
      </button>
    </div>
  )
}
