import { missing } from '../head'
import { link, useTitle } from '../route'
import { start } from '../sections'

export function Missing() {
  useTitle(missing.title)
  return (
    <main className="missing">
      <p className="eyebrow">404</p>
      <h1>No page at this address</h1>
      <p>
        It may have moved. Start from the <a href={link()}>home page</a>, the <a href={start('guide')}>guide</a> or the{' '}
        <a href={link('reference')}>reference</a>.
      </p>
    </main>
  )
}
