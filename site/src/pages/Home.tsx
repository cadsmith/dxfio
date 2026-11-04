import type { CSSProperties } from 'react'
import { BracesIcon, FileCheckIcon, HistoryIcon, ShieldCheckIcon } from 'lucide-react'
import home from '../../../docs/home.md'
import { cn } from 'cn'
import { buttonVariants } from '@/components/ui/button'
import { Stage } from '../components/Hero'
import { Showcase } from '../components/Showcase'
import { Timeline } from '../components/Timeline'
import { home as head } from '../head'
import { link, useTitle } from '../route'
import { start } from '../sections'

const part = (id: string) => home.parts.find((one) => one.id === id)?.html ?? ''

const features = [
  {
    icon: FileCheckIcon,
    title: 'Nothing lost',
    text: 'Every group of every record comes back in its order. A kind with no type of its own is kept whole and written back as it came.',
  },
  {
    icon: HistoryIcon,
    title: 'Every release',
    text: 'Read R10 to R2018 and save to any of them. What an older release lacks is drawn with what it has, as AutoCAD does, or reported.',
  },
  {
    icon: ShieldCheckIcon,
    title: 'Checked by AutoCAD',
    text: '155 drawings AutoCAD 2026 wrote read without an issue and save back with every group kept, and what dxfio writes passes AutoCAD’s own audit.',
  },
  {
    icon: BracesIcon,
    title: 'Plain C++23',
    text: 'Aggregates and variants named after their DXF kinds, std::expected for failures, and edits that keep handles right.',
  },
]

// The title's lines, each word in a mask it rises through as the page first paints.
const title = [
  ['Read', 'and', 'write', 'DXF,'],
  ['nothing', 'lost.'],
]

const order = (at: number): CSSProperties => ({ '--at': at }) as CSSProperties

function Corners() {
  return (
    <>
      <span className="tick tick-tl" aria-hidden="true" />
      <span className="tick tick-tr" aria-hidden="true" />
      <span className="tick tick-bl" aria-hidden="true" />
      <span className="tick tick-br" aria-hidden="true" />
    </>
  )
}

export default function Home() {
  useTitle(head.title)
  return (
    <main className="home">
      <section className="frame hero">
        <Corners />
        <div className="hero-text">
          <p className="hero-eyebrow">C++23 · every release, R10 to R2018 · ASCII and binary</p>
          <h1 className="hero-title">
            {title.map((words, line) => (
              <span key={line} className={line > 0 ? 'hero-line hero-accent' : 'hero-line'}>
                {words.map((word, index) => (
                  <span key={word}>
                    {index > 0 && ' '}
                    <span className="hero-mask">
                      <span className="hero-word" style={order(title.slice(0, line).flat().length + index)}>
                        {word}
                      </span>
                    </span>
                  </span>
                ))}
              </span>
            ))}
          </h1>
          <p className="hero-lead">
            dxfio reads any AutoCAD drawing exchange file into plain C++ values, and writes it back whole or to any
            other release.
          </p>
          <div className="hero-actions">
            <span className="shine">
              <a className={cn(buttonVariants({ size: 'lg', className: 'h-11 rounded-full px-6 text-[15px]' }))} href={start('guide')}>
                Get started
              </a>
            </span>
            <a
              className={cn(buttonVariants({ size: 'lg', variant: 'outline', className: 'h-11 rounded-full px-6 text-[15px]' }))}
              href={link('reference')}
            >
              Reference
            </a>
          </div>
          <div className="hero-install" dangerouslySetInnerHTML={{ __html: part('install') }} />
        </div>
        <Stage />
      </section>

      <section className="frame features" aria-label="What dxfio holds to">
        {features.map((one, index) => (
          <article key={one.title} className="feature" style={order(index)}>
            <span className="feature-icon" aria-hidden="true">
              <one.icon />
            </span>
            <h2>{one.title}</h2>
            <p>{one.text}</p>
          </article>
        ))}
      </section>

      <section className="showcase" aria-labelledby="showcase-title">
        <header className="section-head">
          <p className="eyebrow">In practice</p>
          <h2 id="showcase-title">Load, edit, save</h2>
          <p>Three calls carry a drawing from a file to any release, and the audit says whether AutoCAD will take it.</p>
        </header>
        <Showcase html={part('example')} />
      </section>

      <section className="releases" aria-labelledby="releases-title">
        <header className="section-head">
          <p className="eyebrow">Compatibility</p>
          <h2 id="releases-title">Ten releases, read and written</h2>
          <p>
            Every release AutoCAD has written since 1988 reads into the same document, and saves to any other.{' '}
            <a href={link('concepts', 'releases')}>What each release keeps</a>
          </p>
        </header>
        <Timeline />
      </section>
    </main>
  )
}
